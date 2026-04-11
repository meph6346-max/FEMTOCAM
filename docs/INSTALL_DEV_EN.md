English / [한국어](INSTALL_DEV_KO.md)

# FEMTOCAM Install Guide (Advanced Users)

Build from source and flash to ESP32-CAM.

---

## What You Need

- **AI Thinker ESP32-CAM** + MicroSD
- **ESP32-CAM-MB** or FTDI adapter
- **VS Code** + **PlatformIO** extension
- **Git**

---

## Step 1: Clone

```bash
git clone https://github.com/gorogepapa/femtocam.git
cd femtocam
```

---

## Step 2: Build & Upload

```bash
pio run --target upload
```

> With ESP32-CAM-MB, upload is automatic. With FTDI, connect IO0 to GND before uploading, then disconnect.

---

## Project Structure

```
femtocam/
├── platformio.ini
├── README.md / README_KO.md
├── LICENSE
├── docs/
│   ├── images/
│   ├── INSTALL_EASY_EN.md / INSTALL_EASY_KO.md
│   └── INSTALL_DEV_EN.md / INSTALL_DEV_KO.md
├── include/
│   ├── config.h          Pin map, constants, structs
│   ├── avi_writer.h      MJPEG AVI container
│   └── web_ui.h          Web UI HTML/CSS/JS + i18n
└── src/
    └── main.cpp          Full firmware
```

---

## Key Files

| File | Role | When to modify |
|------|------|----------------|
| `config.h` | Pin map, resolution table, defaults, structs | Add resolution, change defaults |
| `main.cpp` | Full firmware, HTTP handlers | Add endpoints, change logic |
| `web_ui.h` | Web UI (HTML+CSS+JS+i18n) | UI layout, styling, translations |
| `avi_writer.h` | AVI container writer | Video format changes |

---

## API Reference

Web server: port 80 / Stream server: port 81. All endpoints are HTTP GET.

| Endpoint | Description |
|----------|-------------|
| `/` | Web UI |
| `/stream` (port 81) | MJPEG streaming |
| `/status` | System status JSON |
| `/start` · `/stop` | Start / stop recording |
| `/set?res=VGA&capMs=100&pbFps=15&dur=60&hostname=cam1` | Change settings |
| `/flash?on=1` | Flash LED toggle |
| `/files` | File list JSON |
| `/dl?f=filename` | Download file |
| `/rm?f=filename` | Delete file |
| `/rename?f=old&n=new` | Rename file |
| `/cam` · `/camset?brightness=1` | Camera settings get/set |
| `/recparams` · `/recset?sdMinMB=10` | Recording safety params get/set |
| `/log` · `/logclear` · `/loglevel?v=2` | Log view/clear/level |
| `/sysinfo` | System info |
| `/scan` | WiFi scan |
| `/wifi?ssid=...&pass=...` | Save WiFi & reboot |
| `/reset` | Factory reset & reboot |

---

## Adding a New Language

Find the `var L={...}` block inside `<script>` in `web_ui.h`.

1. Copy the `ko` block
2. Change the key (e.g. `ja`, `de`, `zh`)
3. Translate all values
4. Add the new language to the `toggleLang()` cycle

---

## FTDI Wiring (without ESP32-CAM-MB)

| FTDI | ESP32-CAM |
|------|-----------|
| 5V | 5V |
| GND | GND |
| TX | U0R (GPIO 3) |
| RX | U0T (GPIO 1) |
| — | IO0 → GND (flash mode) |

1. Connect IO0 to GND
2. Reconnect power
3. `pio run --target upload`
4. **Remove IO0-GND jumper**
5. Reset to boot normally

---

[← Back to README](../README.md)
