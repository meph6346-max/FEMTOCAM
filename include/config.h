// FEMTOCAM — config.h
// Copyright (c) 2025 Gorogepapa (meph6346@gmail.com) — MIT License
#pragma once
#include "esp_camera.h"

#ifndef FEMTOCAM_VERSION
#define FEMTOCAM_VERSION "2.3.0"
#endif

// --- AI Thinker ESP32-CAM Pins ---
#define PWDN_GPIO 32
#define RESET_GPIO -1
#define XCLK_GPIO 0
#define SIOD_GPIO 26
#define SIOC_GPIO 27
#define Y9_GPIO 35
#define Y8_GPIO 34
#define Y7_GPIO 39
#define Y6_GPIO 36
#define Y5_GPIO 21
#define Y4_GPIO 19
#define Y3_GPIO 18
#define Y2_GPIO 5
#define VSYNC_GPIO 25
#define HREF_GPIO 23
#define PCLK_GPIO 22
#define PIN_LED 33
#define PIN_FLASH 4

// --- WiFi ---
#define AP_SSID "FEMTOCAM"
#define AP_IP IPAddress(192, 168, 4, 1)
#define AP_GW IPAddress(192, 168, 4, 1)
#define AP_MASK IPAddress(255, 255, 255, 0)
#define STA_RETRIES 20
#define STA_TIMEOUT 60000

// --- HTTP ---
#define PORT_STREAM 81
#define PORT_CTRL 80

// --- Resolution ---
struct Res {
    framesize_t fs;
    uint8_t q;
    uint16_t w, h;
    const char* name;
};
const Res RES[] = {
    {FRAMESIZE_QVGA, 12, 320, 240, "QVGA"},
    {FRAMESIZE_VGA, 20, 640, 480, "VGA"},
    {FRAMESIZE_SVGA, 20, 800, 600, "SVGA"},
};
#define RES_COUNT 3

// --- Defaults ---
#define DEF_RES 1         // VGA
#define DEF_CAPTURE_MS 100 // 10fps
#define DEF_PBFPS 15       // playback fps for AVI
#define DEF_DURATION 0     // unlimited
#define SD_MIN_MB 10
#define FAIL_LIMIT 5
#define FLUSH_EVERY 10
#define DEF_BLINK_MS 250

// --- NVS Keys ---
#define NVS_NS "fc"
#define K_SSID "ss"
#define K_PASS "pw"
#define K_IPMODE "im"
#define K_IP "ip"
#define K_GW "gw"
#define K_MASK "mk"
#define K_HOST "hn"
#define K_RES "rs"
#define K_CAPMS "cm"
#define K_PBFPS "pf"
#define K_DUR "du"
#define K_LANG "ln"

// --- Camera tunable params ---
struct CamParams {
    int8_t brightness;   // -2~2
    int8_t contrast;     // -2~2
    int8_t saturation;   // -2~2
    uint8_t quality;     // 4~63 (lower=better quality, bigger file)
    uint8_t gainceiling; // 0~6 (2x~128x, higher=brighter in dark but more noise)
    uint8_t wb;          // 0/1 auto white balance
    uint8_t hmirror;     // 0/1
    uint8_t vflip;       // 0/1
};
#define CAM_DEFAULTS \
    { 1, 0, -1, 20, 2, 1, 0, 0 }

// --- Recording safety params ---
struct RecParams {
    uint16_t sdMinMB;    // min SD free MB to start/continue
    uint8_t failLimit;   // consecutive capture fail limit
    uint8_t flushEvery;  // flush AVI every N frames (0=disable)
};
#define REC_DEFAULTS \
    { SD_MIN_MB, FAIL_LIMIT, FLUSH_EVERY }

// --- Log ---
#define LOG_BUF_LINES 80
#define LOG_LINE_MAX 120
