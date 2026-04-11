// FEMTOCAM v2.3.0
// Copyright (c) 2025 Gorogepapa (meph6346@gmail.com) — MIT License
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <esp_camera.h>
#include <esp_http_server.h>
#include <esp_task_wdt.h>
#include <sys/socket.h>
#include <time.h>
#include "SD_MMC.h"
#include "config.h"
#include "avi_writer.h"
#include "web_ui.h"

// ======== Log Ring Buffer ========
char logBuf[LOG_BUF_LINES][LOG_LINE_MAX];
uint16_t logHead = 0, logCount = 0;
uint8_t logLevel = 1; // 0=min,1=normal,2=verbose

void logMsg(uint8_t level, const char* fmt, ...) {
    if (level > logLevel) return;
    va_list args;
    va_start(args, fmt);
    // Write to serial
    char tmp[LOG_LINE_MAX];
    vsnprintf(tmp, LOG_LINE_MAX, fmt, args);
    va_end(args);
    Serial.println(tmp);
    // Write to ring buffer with timestamp
    unsigned long s = millis() / 1000;
    snprintf(logBuf[logHead], LOG_LINE_MAX, "[%lu:%02lu] %s", s / 60, s % 60, tmp);
    logHead = (logHead + 1) % LOG_BUF_LINES;
    if (logCount < LOG_BUF_LINES) logCount++;
}

// ======== State ========
Preferences prefs;
httpd_handle_t hStream = NULL, hCtrl = NULL;
AviWriter avi;
bool isAP = false, flash_on = false, sd_ok = false, ntpOK = false;
unsigned long bootMs = 0;
CamParams cam = CAM_DEFAULTS;
RecParams rp = REC_DEFAULTS;

struct {
    String ssid, pass, sip, sgw, smask, host;
    uint8_t ipm, res, lang;
    uint16_t dur;
    uint32_t capMs;
    uint8_t pbFps;
} cfg;

struct {
    bool on;
    uint32_t frames, fails;
    char fn[48];
    TimerHandle_t tmr;
    unsigned long t0;
    uint16_t durMin;
} tl = {};

// ======== LED ========
TaskHandle_t blinkH = NULL;
void ledOn() { digitalWrite(PIN_LED, LOW); }
void ledOff() { digitalWrite(PIN_LED, HIGH); }
static void blinkTask(void*) {
    for (;;) {
        digitalWrite(PIN_LED, LOW);
        vTaskDelay(pdMS_TO_TICKS(DEF_BLINK_MS));
        digitalWrite(PIN_LED, HIGH);
        vTaskDelay(pdMS_TO_TICKS(DEF_BLINK_MS));
    }
}
void blinkStart() { if (!blinkH) xTaskCreate(blinkTask, "bk", 1024, NULL, 1, &blinkH); }
void blinkStop() { if (blinkH) { vTaskDelete(blinkH); blinkH = NULL; } }
void flashPulse(int n, int ms) {
    for (int i = 0; i < n; i++) { digitalWrite(PIN_FLASH, 1); delay(ms); digitalWrite(PIN_FLASH, 0); delay(ms); }
    digitalWrite(PIN_FLASH, flash_on);
}

// ======== Util ========
String fmtUp(unsigned long ms) {
    unsigned long s = ms / 1000, m = (s / 60) % 60, h = (s / 3600) % 24, d = s / 86400;
    char b[24];
    if (d) snprintf(b, 24, "%lud%luh", d, h);
    else if (h) snprintf(b, 24, "%luh%lum", h, m);
    else snprintf(b, 24, "%lum%lus", m, s % 60);
    return b;
}
String fmtB(uint64_t b) {
    char s[16];
    if (b >= 1073741824ULL) snprintf(s, 16, "%.1fGB", b / 1073741824.0);
    else if (b >= 1048576) snprintf(s, 16, "%.1fMB", b / 1048576.0);
    else snprintf(s, 16, "%.1fKB", b / 1024.0);
    return s;
}
uint64_t sdFree() { return sd_ok ? (SD_MMC.totalBytes() - SD_MMC.usedBytes()) / 1048576 : 0; }

// ======== NVS ========
void loadCfg() {
    prefs.begin(NVS_NS, true);
    cfg.ssid = prefs.getString(K_SSID, ""); cfg.pass = prefs.getString(K_PASS, "");
    cfg.ipm = prefs.getUChar(K_IPMODE, 0); cfg.sip = prefs.getString(K_IP, "");
    cfg.sgw = prefs.getString(K_GW, ""); cfg.smask = prefs.getString(K_MASK, "255.255.255.0");
    cfg.host = prefs.getString(K_HOST, "femtocam");
    cfg.res = prefs.getUChar(K_RES, DEF_RES);
    cfg.capMs = prefs.getULong(K_CAPMS, DEF_CAPTURE_MS);
    cfg.pbFps = prefs.getUChar(K_PBFPS, DEF_PBFPS);
    cfg.dur = prefs.getUShort(K_DUR, DEF_DURATION);
    cfg.lang = prefs.getUChar(K_LANG, 0);
    prefs.end();
    if (cfg.res >= RES_COUNT) cfg.res = DEF_RES;
    if (cfg.capMs < 42) cfg.capMs = 42;
    if (cfg.capMs > 3600000UL) cfg.capMs = 3600000UL;
    if (cfg.pbFps < 1) cfg.pbFps = 1;
    if (cfg.pbFps > 60) cfg.pbFps = 60;
}
void saveCfg() {
    prefs.begin(NVS_NS, false);
    prefs.putString(K_SSID, cfg.ssid); prefs.putString(K_PASS, cfg.pass);
    prefs.putUChar(K_IPMODE, cfg.ipm); prefs.putString(K_IP, cfg.sip);
    prefs.putString(K_GW, cfg.sgw); prefs.putString(K_MASK, cfg.smask);
    prefs.putString(K_HOST, cfg.host); prefs.putUChar(K_RES, cfg.res);
    prefs.putULong(K_CAPMS, cfg.capMs); prefs.putUChar(K_PBFPS, cfg.pbFps);
    prefs.putUShort(K_DUR, cfg.dur); prefs.putUChar(K_LANG, cfg.lang);
    prefs.end();
}

// ======== WiFi ========
void setupWiFi() {
    WiFi.persistent(false); WiFi.setSleep(false);
    if (cfg.ssid.length() > 0) {
        logMsg(1, "[WiFi] STA: %s", cfg.ssid.c_str()); blinkStart();
        WiFi.mode(WIFI_STA); WiFi.setHostname(cfg.host.c_str()); WiFi.setAutoReconnect(true);
        WiFi.setTxPower(WIFI_POWER_17dBm);
        if (cfg.ipm == 1 && cfg.sip.length()) { IPAddress a, g, m; a.fromString(cfg.sip); g.fromString(cfg.sgw); m.fromString(cfg.smask); WiFi.config(a, g, m); }
        WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
        for (int i = 0; i < STA_RETRIES && WiFi.status() != WL_CONNECTED; i++) { delay(500); }
        blinkStop();
        if (WiFi.status() == WL_CONNECTED) {
            isAP = false; ledOff();
            logMsg(1, "[WiFi] OK: %s RSSI:%d", WiFi.localIP().toString().c_str(), WiFi.RSSI());
            if (MDNS.begin(cfg.host.c_str())) MDNS.addService("http", "tcp", PORT_CTRL);
            return;
        }
        logMsg(1, "[WiFi] fail->AP");
    }
    isAP = true; ledOn(); WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK); WiFi.softAP(AP_SSID, NULL, 1, 0, 4);
    WiFi.setTxPower(WIFI_POWER_17dBm);
    logMsg(1, "[WiFi] AP %s", WiFi.softAPIP().toString().c_str());
}

// ======== NTP ========
void setupNTP() {
    if (isAP) return;
    configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    for (int i = 0; i < 10; i++) {
        if (time(NULL) > 1700000000) { ntpOK = true; logMsg(1, "[NTP] OK"); return; }
        delay(500);
    }
    logMsg(1, "[NTP] fail");
}

// ======== Filename ========
void makeFilename() {
    if (ntpOK) {
        time_t now = time(NULL); struct tm* t = localtime(&now);
        snprintf(tl.fn, 48, "/%04d%02d%02d_%02d%02d%02d.avi",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    } else {
        int maxN = 0;
        if (sd_ok) {
            File root = SD_MMC.open("/"); if (root) {
                File f = root.openNextFile();
                while (f) { String nm = f.name(); int idx = nm.indexOf("aprec_");
                    if (idx >= 0) { int n = nm.substring(idx + 6, nm.indexOf('.', idx)).toInt(); if (n > maxN) maxN = n; }
                    f = root.openNextFile(); }
            }
        }
        snprintf(tl.fn, 48, "/aprec_%03d.avi", maxN + 1);
    }
}

// ======== Camera ========
void applyCam() {
    sensor_t* s = esp_camera_sensor_get(); if (!s) return;
    s->set_brightness(s, cam.brightness); s->set_contrast(s, cam.contrast);
    s->set_saturation(s, cam.saturation); s->set_quality(s, cam.quality);
    s->set_gainceiling(s, (gainceiling_t)cam.gainceiling);
    s->set_whitebal(s, cam.wb); s->set_hmirror(s, cam.hmirror); s->set_vflip(s, cam.vflip);
}
bool initCam() {
    camera_config_t c = {};
    c.ledc_channel = LEDC_CHANNEL_0; c.ledc_timer = LEDC_TIMER_0;
    c.pin_d0 = Y2_GPIO; c.pin_d1 = Y3_GPIO; c.pin_d2 = Y4_GPIO; c.pin_d3 = Y5_GPIO;
    c.pin_d4 = Y6_GPIO; c.pin_d5 = Y7_GPIO; c.pin_d6 = Y8_GPIO; c.pin_d7 = Y9_GPIO;
    c.pin_xclk = XCLK_GPIO; c.pin_pclk = PCLK_GPIO; c.pin_vsync = VSYNC_GPIO; c.pin_href = HREF_GPIO;
    c.pin_sccb_sda = SIOD_GPIO; c.pin_sccb_scl = SIOC_GPIO; c.pin_pwdn = PWDN_GPIO; c.pin_reset = RESET_GPIO;
    c.xclk_freq_hz = 16000000; c.pixel_format = PIXFORMAT_JPEG;
    c.frame_size = RES[cfg.res].fs; c.jpeg_quality = RES[cfg.res].q;
    c.fb_count = psramFound() ? 3 : 1; c.grab_mode = CAMERA_GRAB_LATEST;
    if (esp_camera_init(&c) != ESP_OK) return false;
    applyCam(); return true;
}
bool initSD() { if (!SD_MMC.begin("/sdcard", true) || SD_MMC.cardType() == CARD_NONE) return false; sd_ok = true; return true; }

// ======== Path helper ========
void ensureSlash(const char* in, char* out, size_t sz) {
    if (in[0] != '/') snprintf(out, sz, "/%s", in); else strncpy(out, in, sz);
}

// ======== Safe stop ========
void stopRec() {
    if (!tl.on) return;
    if (tl.tmr) xTimerStop(tl.tmr, 0); avi.finalize(); tl.on = false; flashPulse(2, 100);
    logMsg(1, "[REC] Done %s %u frames", tl.fn, tl.frames);
    tl.frames = 0; tl.fails = 0; tl.t0 = 0; tl.durMin = 0;
}

// ======== HTTP: Stream (raw socket) ========
#define BDY "fb"
static esp_err_t h_stream(httpd_req_t* r) {
    const char* hdr = "HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace;boundary=" BDY "\r\n"
        "Access-Control-Allow-Origin: *\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n";
    int fd = httpd_req_to_sockfd(r); if (fd < 0) return ESP_FAIL;
    struct timeval tv = {.tv_sec = 10, .tv_usec = 0}; setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    if (send(fd, hdr, strlen(hdr), 0) <= 0) return ESP_FAIL;
    char ph[64];
    while (1) {
        camera_fb_t* f = esp_camera_fb_get();
        if (!f) { vTaskDelay(pdMS_TO_TICKS(5)); continue; }
        int hl = snprintf(ph, 64, "\r\n--" BDY "\r\nContent-Type:image/jpeg\r\nContent-Length:%u\r\n\r\n", f->len);
        if (send(fd, ph, hl, 0) <= 0) { esp_camera_fb_return(f); break; }
        const uint8_t* p = f->buf; size_t rem = f->len; bool ok = true;
        while (rem > 0) { size_t ch = rem > 4096 ? 4096 : rem; int s = send(fd, (const char*)p, ch, 0);
            if (s <= 0) { ok = false; break; } p += s; rem -= s; }
        esp_camera_fb_return(f); if (!ok) break;
        vTaskDelay(pdMS_TO_TICKS(150));
    }
    return ESP_OK;
}

// ======== HTTP: Status ========
static esp_err_t h_status(httpd_req_t* r) {
    char j[700]; unsigned long el = tl.on ? (millis() - tl.t0) / 1000 : 0; String up = fmtUp(millis() - bootMs);
    snprintf(j, 700, "{\"mode\":\"%s\",\"ssid\":\"%s\",\"ip\":\"%s\",\"recording\":%s,\"frameCount\":%u,"
        "\"recElapsed\":%lu,\"resolution\":\"%s\",\"capMs\":%lu,\"pbFps\":%u,\"duration\":%u,\"flash\":%s,"
        "\"freeSpace\":%llu,\"sdReady\":%s,\"hostname\":\"%s\",\"freeHeap\":%u,"
        "\"firmware\":\"" FEMTOCAM_VERSION "\",\"uptime\":\"%s\",\"rssi\":%d,"
        "\"psram\":%s,\"psramFree\":%u,\"lang\":%u}",
        isAP ? "AP" : "STA", isAP ? AP_SSID : cfg.ssid.c_str(),
        isAP ? WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str(),
        tl.on ? "true" : "false", tl.frames, el, RES[cfg.res].name, cfg.capMs, cfg.pbFps, cfg.dur,
        flash_on ? "true" : "false", sdFree(), sd_ok ? "true" : "false",
        cfg.host.c_str(), ESP.getFreeHeap(), up.c_str(),
        isAP ? 0 : WiFi.RSSI(), psramFound() ? "true" : "false", psramFound() ? ESP.getFreePsram() : 0u, cfg.lang);
    httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    return httpd_resp_sendstr(r, j);
}

// ======== HTTP: Flash ========
static esp_err_t h_flash(httpd_req_t* r) {
    char q[32], v[4];
    if (httpd_req_get_url_query_str(r, q, 32) == ESP_OK && httpd_query_key_value(q, "on", v, 4) == ESP_OK) {
        flash_on = (v[0] == '1'); digitalWrite(PIN_FLASH, flash_on); }
    return httpd_resp_sendstr(r, flash_on ? "ON" : "OFF");
}

// ======== HTTP: Start/Stop ========
static esp_err_t h_start(httpd_req_t* r) {
    if (tl.on) return httpd_resp_sendstr(r, "ERR:already recording");
    if (!sd_ok) return httpd_resp_sendstr(r, "ERR:no SD card");
    if (sdFree() < rp.sdMinMB) return httpd_resp_sendstr(r, "ERR:SD full");
    makeFilename();
    if (!avi.begin(tl.fn, RES[cfg.res].w, RES[cfg.res].h, cfg.pbFps)) return httpd_resp_sendstr(r, "ERR:file error");
    tl.on = true; tl.frames = 0; tl.fails = 0; tl.t0 = millis(); tl.durMin = cfg.dur;
    if (tl.tmr) { xTimerChangePeriod(tl.tmr, pdMS_TO_TICKS(cfg.capMs), 0); xTimerStart(tl.tmr, 0); }
    flashPulse(3, 80);
    logMsg(1, "[REC] Start %s pbFps=%u capMs=%lu dur=%umin", tl.fn, cfg.pbFps, cfg.capMs, cfg.dur);
    return httpd_resp_sendstr(r, "OK");
}
static esp_err_t h_stop(httpd_req_t* r) { stopRec(); return httpd_resp_sendstr(r, "OK"); }

// ======== HTTP: Set (recording params) ========
static esp_err_t h_set(httpd_req_t* r) {
    char q[200], v[16], vh[64];
    if (httpd_req_get_url_query_str(r, q, 200) != ESP_OK) return httpd_resp_sendstr(r, "ERR:no params");
    if (httpd_query_key_value(q, "res", v, 16) == ESP_OK) {
        if (tl.on) return httpd_resp_sendstr(r, "ERR:recording");
        for (int i = 0; i < RES_COUNT; i++) if (strcmp(v, RES[i].name) == 0) {
            sensor_t* s = esp_camera_sensor_get(); s->set_framesize(s, RES[i].fs); s->set_quality(s, RES[i].q); cfg.res = i; break; } }
    if (httpd_query_key_value(q, "capMs", v, 16) == ESP_OK) { uint32_t n = strtoul(v, NULL, 10); if (n >= 42 && n <= 3600000UL) cfg.capMs = n; }
    if (httpd_query_key_value(q, "pbFps", v, 8) == ESP_OK) { int n = atoi(v); if (n >= 1 && n <= 60) cfg.pbFps = n; }
    if (httpd_query_key_value(q, "dur", v, 16) == ESP_OK) { uint16_t n = atoi(v); if (n <= 600) cfg.dur = n; }
    if (httpd_query_key_value(q, "lang", v, 4) == ESP_OK) cfg.lang = atoi(v);
    if (httpd_query_key_value(q, "hostname", vh, 64) == ESP_OK) { String h = vh; h.replace(" ", "_"); h.replace("%20", "_"); cfg.host = h; }
    saveCfg(); return httpd_resp_sendstr(r, "OK");
}

// ======== HTTP: Index ========
static esp_err_t h_index(httpd_req_t* r) { httpd_resp_set_type(r, "text/html"); return httpd_resp_send(r, INDEX_HTML, strlen(INDEX_HTML)); }

// ======== HTTP: Camera settings ========
static esp_err_t h_cam_get(httpd_req_t* r) {
    char j[256]; snprintf(j, 256, "{\"brightness\":%d,\"contrast\":%d,\"saturation\":%d,\"quality\":%u,"
        "\"gainceiling\":%u,\"wb\":%u,\"hmirror\":%u,\"vflip\":%u}",
        cam.brightness, cam.contrast, cam.saturation, cam.quality, cam.gainceiling, cam.wb, cam.hmirror, cam.vflip);
    httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    return httpd_resp_sendstr(r, j);
}
static esp_err_t h_cam_set(httpd_req_t* r) {
    char q[200], v[8];
    if (httpd_req_get_url_query_str(r, q, 200) != ESP_OK) return httpd_resp_sendstr(r, "ERR:no params");
    if (httpd_query_key_value(q, "brightness", v, 8) == ESP_OK) cam.brightness = atoi(v);
    if (httpd_query_key_value(q, "contrast", v, 8) == ESP_OK) cam.contrast = atoi(v);
    if (httpd_query_key_value(q, "saturation", v, 8) == ESP_OK) cam.saturation = atoi(v);
    if (httpd_query_key_value(q, "quality", v, 8) == ESP_OK) { int n = atoi(v); if (n >= 4 && n <= 63) cam.quality = n; }
    if (httpd_query_key_value(q, "gainceiling", v, 8) == ESP_OK) { int n = atoi(v); if (n >= 0 && n <= 6) cam.gainceiling = n; }
    if (httpd_query_key_value(q, "wb", v, 4) == ESP_OK) cam.wb = atoi(v);
    if (httpd_query_key_value(q, "hmirror", v, 4) == ESP_OK) cam.hmirror = atoi(v);
    if (httpd_query_key_value(q, "vflip", v, 4) == ESP_OK) cam.vflip = atoi(v);
    if (httpd_query_key_value(q, "reset", v, 4) == ESP_OK && v[0] == '1') cam = (CamParams)CAM_DEFAULTS;
    applyCam();
    logMsg(2, "[CAM] b:%d c:%d s:%d q:%u gc:%u", cam.brightness, cam.contrast, cam.saturation, cam.quality, cam.gainceiling);
    return httpd_resp_sendstr(r, "OK");
}

// ======== HTTP: Recording safety params ========
static esp_err_t h_rec_get(httpd_req_t* r) {
    char j[128]; snprintf(j, 128, "{\"sdMinMB\":%u,\"failLimit\":%u,\"flushEvery\":%u}", rp.sdMinMB, rp.failLimit, rp.flushEvery);
    httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    return httpd_resp_sendstr(r, j);
}
static esp_err_t h_rec_set(httpd_req_t* r) {
    char q[128], v[16];
    if (httpd_req_get_url_query_str(r, q, 128) != ESP_OK) return httpd_resp_sendstr(r, "ERR:no params");
    if (httpd_query_key_value(q, "sdMinMB", v, 16) == ESP_OK) { int n = atoi(v); if (n >= 1 && n <= 1000) rp.sdMinMB = n; }
    if (httpd_query_key_value(q, "failLimit", v, 16) == ESP_OK) { int n = atoi(v); if (n >= 1 && n <= 100) rp.failLimit = n; }
    if (httpd_query_key_value(q, "flushEvery", v, 16) == ESP_OK) { int n = atoi(v); if (n >= 0 && n <= 1000) rp.flushEvery = n; }
    if (httpd_query_key_value(q, "reset", v, 4) == ESP_OK && v[0] == '1') rp = (RecParams)REC_DEFAULTS;
    return httpd_resp_sendstr(r, "OK");
}

// ======== HTTP: Log ========
static esp_err_t h_log(httpd_req_t* r) {
    httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    String j = "[";
    uint16_t start = (logCount < LOG_BUF_LINES) ? 0 : logHead;
    for (uint16_t i = 0; i < logCount; i++) {
        uint16_t idx = (start + i) % LOG_BUF_LINES;
        if (i > 0) j += ",";
        String line = logBuf[idx]; line.replace("\"", "\\\"");
        j += "\"" + line + "\"";
    }
    j += "]"; return httpd_resp_sendstr(r, j.c_str());
}
static esp_err_t h_logclear(httpd_req_t* r) { logHead = 0; logCount = 0; return httpd_resp_sendstr(r, "OK"); }
static esp_err_t h_loglevel(httpd_req_t* r) {
    char q[32], v[4];
    if (httpd_req_get_url_query_str(r, q, 32) == ESP_OK && httpd_query_key_value(q, "v", v, 4) == ESP_OK) {
        int n = atoi(v); if (n >= 0 && n <= 2) logLevel = n; }
    char j[32]; snprintf(j, 32, "{\"level\":%u}", logLevel);
    httpd_resp_set_type(r, "application/json"); return httpd_resp_sendstr(r, j);
}

// ======== HTTP: System info ========
static esp_err_t h_sysinfo(httpd_req_t* r) {
    char j[400]; String up = fmtUp(millis() - bootMs);
    snprintf(j, 400, "{\"firmware\":\"" FEMTOCAM_VERSION "\",\"uptime\":\"%s\",\"freeHeap\":%u,"
        "\"psram\":%s,\"psramFree\":%u,\"sdFree\":%llu,\"sdReady\":%s,"
        "\"wifiMode\":\"%s\",\"rssi\":%d,\"hostname\":\"%s\",\"ip\":\"%s\",\"ntpOK\":%s}",
        up.c_str(), ESP.getFreeHeap(), psramFound() ? "true" : "false", psramFound() ? ESP.getFreePsram() : 0u,
        sdFree(), sd_ok ? "true" : "false", isAP ? "AP" : "STA", isAP ? 0 : WiFi.RSSI(),
        cfg.host.c_str(), isAP ? WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str(),
        ntpOK ? "true" : "false");
    httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    return httpd_resp_sendstr(r, j);
}

// ======== HTTP: Files ========
static esp_err_t h_files(httpd_req_t* r) {
    httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    if (!sd_ok) return httpd_resp_sendstr(r, "[]");
    File root = SD_MMC.open("/"); if (!root) return httpd_resp_sendstr(r, "[]");
    String j = "["; bool first = true; File f = root.openNextFile();
    while (f) { if (!f.isDirectory()) { String n = f.name(); if (n.endsWith(".avi")) { if (!first) j += ",";
        j += "{\"name\":\"" + n + "\",\"size\":\"" + fmtB(f.size()) + "\",\"recording\":" +
            String(tl.on && n == String(tl.fn) ? "true" : "false") + "}"; first = false; } } f = root.openNextFile(); }
    j += "]"; return httpd_resp_sendstr(r, j.c_str());
}
static esp_err_t h_dl(httpd_req_t* r) {
    char q[64], fn[48], path[50];
    if (httpd_req_get_url_query_str(r, q, 64) != ESP_OK || httpd_query_key_value(q, "f", fn, 48) != ESP_OK) return httpd_resp_send_err(r, HTTPD_400_BAD_REQUEST, "no file");
    ensureSlash(fn, path, 50);
    if (tl.on && strcmp(path, tl.fn) == 0) return httpd_resp_sendstr(r, "ERR:recording");
    File f = SD_MMC.open(path); if (!f) return httpd_resp_send_err(r, HTTPD_404_NOT_FOUND, "not found");
    httpd_resp_set_type(r, "application/octet-stream");
    char h[80]; snprintf(h, 80, "attachment;filename=\"%s\"", path[0] == '/' ? path + 1 : path);
    httpd_resp_set_hdr(r, "Content-Disposition", h);
    uint8_t* buf = (uint8_t*)(psramFound() ? ps_malloc(16384) : malloc(4096)); size_t bsz = psramFound() ? 16384 : 4096;
    if (!buf) { f.close(); return httpd_resp_send_err(r, HTTPD_500_INTERNAL_SERVER_ERROR, "oom"); }
    size_t rd; while ((rd = f.read(buf, bsz)) > 0) if (httpd_resp_send_chunk(r, (char*)buf, rd) != ESP_OK) break;
    free(buf); f.close(); httpd_resp_send_chunk(r, NULL, 0); return ESP_OK;
}
static esp_err_t h_rm(httpd_req_t* r) {
    char q[64], fn[48], path[50];
    if (httpd_req_get_url_query_str(r, q, 64) != ESP_OK || httpd_query_key_value(q, "f", fn, 48) != ESP_OK) return httpd_resp_send_err(r, HTTPD_400_BAD_REQUEST, "no file");
    ensureSlash(fn, path, 50);
    if (tl.on && strcmp(path, tl.fn) == 0) return httpd_resp_sendstr(r, "ERR:recording");
    bool ok = SD_MMC.remove(path); if (ok) logMsg(1, "[FS] Del %s", path);
    return httpd_resp_sendstr(r, ok ? "OK" : "ERR:failed");
}
static esp_err_t h_rename(httpd_req_t* r) {
    char q[160], fo[48], fn[48], po[50], pn[50];
    if (httpd_req_get_url_query_str(r, q, 160) != ESP_OK) return httpd_resp_sendstr(r, "ERR:no params");
    if (httpd_query_key_value(q, "f", fo, 48) != ESP_OK || httpd_query_key_value(q, "n", fn, 48) != ESP_OK)
        return httpd_resp_sendstr(r, "ERR:need f and n");
    ensureSlash(fo, po, 50); ensureSlash(fn, pn, 50);
    if (!strstr(pn, ".avi")) strncat(pn, ".avi", 50 - strlen(pn) - 1);
    if (tl.on && strcmp(po, tl.fn) == 0) return httpd_resp_sendstr(r, "ERR:recording");
    if (SD_MMC.exists(pn)) return httpd_resp_sendstr(r, "ERR:name exists");
    bool ok = SD_MMC.rename(po, pn); if (ok) logMsg(1, "[FS] Ren %s -> %s", po, pn);
    return httpd_resp_sendstr(r, ok ? "OK" : "ERR:rename failed");
}

// ======== HTTP: WiFi ========
static esp_err_t h_scan(httpd_req_t* r) {
    int n = WiFi.scanNetworks(false, false, false, 300);
    struct AP { String ssid; int rssi; bool enc; }; AP best[20]; int cnt = 0;
    for (int i = 0; i < n; i++) { String s = WiFi.SSID(i); if (!s.length()) continue; bool found = false;
        for (int k = 0; k < cnt; k++) if (best[k].ssid == s) { if (WiFi.RSSI(i) > best[k].rssi) best[k].rssi = WiFi.RSSI(i); found = true; break; }
        if (!found && cnt < 20) { best[cnt++] = {s, WiFi.RSSI(i), WiFi.encryptionType(i) != WIFI_AUTH_OPEN}; } }
    for (int i = 0; i < cnt - 1; i++) for (int k = i + 1; k < cnt; k++) if (best[k].rssi > best[i].rssi) { AP t = best[i]; best[i] = best[k]; best[k] = t; }
    String j = "["; for (int i = 0; i < cnt; i++) { if (i) j += ","; String s = best[i].ssid; s.replace("\\", "\\\\"); s.replace("\"", "\\\"");
        j += "{\"ssid\":\"" + s + "\",\"rssi\":" + String(best[i].rssi) + ",\"enc\":" + String(best[i].enc ? 1 : 0) + "}"; }
    j += "]"; WiFi.scanDelete(); httpd_resp_set_type(r, "application/json"); httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");
    return httpd_resp_sendstr(r, j.c_str());
}
static esp_err_t h_wifi(httpd_req_t* r) {
    char q[400], v[128];
    if (httpd_req_get_url_query_str(r, q, 400) != ESP_OK) return httpd_resp_sendstr(r, "ERR:no params");
    if (httpd_query_key_value(q, "ssid", v, 128) == ESP_OK) cfg.ssid = v;
    if (httpd_query_key_value(q, "pass", v, 128) == ESP_OK) cfg.pass = v;
    if (httpd_query_key_value(q, "hostname", v, 128) == ESP_OK) { String h = v; h.replace(" ", "_"); h.replace("%20", "_"); cfg.host = h; }
    if (httpd_query_key_value(q, "ip_mode", v, 8) == ESP_OK) cfg.ipm = atoi(v);
    if (httpd_query_key_value(q, "ip", v, 32) == ESP_OK) cfg.sip = v;
    if (httpd_query_key_value(q, "gw", v, 32) == ESP_OK) cfg.sgw = v;
    if (httpd_query_key_value(q, "sn", v, 32) == ESP_OK) cfg.smask = v;
    stopRec(); saveCfg(); httpd_resp_sendstr(r, "OK:rebooting"); delay(500); ESP.restart(); return ESP_OK;
}
static esp_err_t h_reset(httpd_req_t* r) {
    stopRec(); prefs.begin(NVS_NS, false); prefs.clear(); prefs.end();
    httpd_resp_sendstr(r, "OK:rebooting"); delay(500); ESP.restart(); return ESP_OK;
}

// ======== Servers ========
void startServers() {
    httpd_config_t sc = HTTPD_DEFAULT_CONFIG(); sc.server_port = PORT_STREAM; sc.ctrl_port = 32768;
    sc.max_open_sockets = 1; sc.stack_size = 16384; sc.send_wait_timeout = 10; sc.recv_wait_timeout = 10;
    sc.lru_purge_enable = true;
    if (httpd_start(&hStream, &sc) == ESP_OK) { httpd_uri_t u = {"/stream", HTTP_GET, h_stream, NULL}; httpd_register_uri_handler(hStream, &u); }

    httpd_config_t cc = HTTPD_DEFAULT_CONFIG(); cc.server_port = PORT_CTRL; cc.ctrl_port = 32769;
    cc.stack_size = 12288; cc.max_uri_handlers = 24; cc.max_open_sockets = 4; cc.lru_purge_enable = true;
    if (httpd_start(&hCtrl, &cc) == ESP_OK) {
        httpd_uri_t u[] = {
            {"/", HTTP_GET, h_index, 0}, {"/status", HTTP_GET, h_status, 0}, {"/flash", HTTP_GET, h_flash, 0},
            {"/start", HTTP_GET, h_start, 0}, {"/stop", HTTP_GET, h_stop, 0}, {"/set", HTTP_GET, h_set, 0},
            {"/files", HTTP_GET, h_files, 0}, {"/dl", HTTP_GET, h_dl, 0}, {"/rm", HTTP_GET, h_rm, 0},
            {"/rename", HTTP_GET, h_rename, 0},
            {"/scan", HTTP_GET, h_scan, 0}, {"/wifi", HTTP_GET, h_wifi, 0}, {"/reset", HTTP_GET, h_reset, 0},
            {"/cam", HTTP_GET, h_cam_get, 0}, {"/camset", HTTP_GET, h_cam_set, 0},
            {"/recparams", HTTP_GET, h_rec_get, 0}, {"/recset", HTTP_GET, h_rec_set, 0},
            {"/log", HTTP_GET, h_log, 0}, {"/logclear", HTTP_GET, h_logclear, 0}, {"/loglevel", HTTP_GET, h_loglevel, 0},
            {"/sysinfo", HTTP_GET, h_sysinfo, 0},
        };
        for (auto& x : u) httpd_register_uri_handler(hCtrl, &x);
        logMsg(1, "[HTTP] %d endpoints", (int)(sizeof(u) / sizeof(u[0])));
    }
}

// ======== Capture callback ========
void captureCB(TimerHandle_t) {
    if (!tl.on) return;
    if (tl.durMin > 0 && (millis() - tl.t0) >= tl.durMin * 60000UL) { logMsg(1, "[REC] timer done"); stopRec(); return; }
    if (sdFree() < rp.sdMinMB) { logMsg(1, "[REC] SD full"); stopRec(); return; }
    camera_fb_t* f = esp_camera_fb_get();
    if (f) { if (avi.addFrame(f->buf, f->len)) { tl.frames++; tl.fails = 0;
        if (rp.flushEvery > 0 && tl.frames % rp.flushEvery == 0) avi.flush(); } else tl.fails++;
        esp_camera_fb_return(f); } else tl.fails++;
    if (tl.fails >= rp.failLimit) { logMsg(1, "[REC] %u fails", tl.fails); stopRec(); }
    if (logLevel >= 2 && tl.frames % 10 == 0) logMsg(2, "[REC] #%u H:%u", tl.frames, ESP.getFreeHeap());
}

// ======== Main ========
void setup() {
    Serial.begin(115200); bootMs = millis();
    logMsg(1, "==== FEMTOCAM v" FEMTOCAM_VERSION " ====");
    pinMode(PIN_LED, OUTPUT); pinMode(PIN_FLASH, OUTPUT); ledOff(); digitalWrite(PIN_FLASH, 0);
    loadCfg();
    logMsg(1, "[Cfg] %s capMs=%lu pbFps=%u host=%s", RES[cfg.res].name, cfg.capMs, cfg.pbFps, cfg.host.c_str());
    initSD() ? logMsg(1, "[SD] %s free", fmtB(SD_MMC.totalBytes() - SD_MMC.usedBytes()).c_str()) : logMsg(1, "[SD] fail");
    if (!initCam()) { logMsg(0, "[Cam] FAIL"); for (;;) { ledOn(); delay(200); ledOff(); delay(200); } }
    logMsg(1, "[Cam] OK PSRAM:%s", psramFound() ? "Y" : "N");
    setupWiFi(); setupNTP(); startServers();
    tl.tmr = xTimerCreate("cap", pdMS_TO_TICKS(cfg.capMs), pdTRUE, 0, captureCB);
    esp_task_wdt_init(60, true); esp_task_wdt_add(NULL);
    logMsg(1, "[OK] http://%s:%d/", isAP ? WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str(), PORT_CTRL);
}

void loop() {
    esp_task_wdt_reset();
    if (!isAP && WiFi.status() != WL_CONNECTED) {
        blinkStart(); unsigned long t = millis(); WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED && millis() - t < STA_TIMEOUT) { esp_task_wdt_reset(); delay(500); }
        blinkStop();
        if (WiFi.status() == WL_CONNECTED) { ledOff(); logMsg(1, "[WiFi] reconnected RSSI:%d", WiFi.RSSI()); }
        else { stopRec(); WiFi.disconnect(); WiFi.mode(WIFI_AP); WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK);
            WiFi.softAP(AP_SSID, NULL, 1, 0, 4); WiFi.setTxPower(WIFI_POWER_17dBm); isAP = true; ledOn();
            logMsg(1, "[WiFi] fallback to AP"); }
    }
    static unsigned long ll = 0;
    if (logLevel >= 1 && millis() - ll > 60000) { ll = millis(); logMsg(1, "[Stat] H:%u SD:%lluMB R:%s", ESP.getFreeHeap(), sdFree(), tl.on ? "Y" : "N"); }
    delay(1000);
}
