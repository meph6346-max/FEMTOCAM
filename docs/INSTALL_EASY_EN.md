English / [한국어](INSTALL_EASY_KO.md)

# FEMTOCAM Install Guide (Regular Users)

Flash pre-built firmware to ESP32-CAM-MB — no compiling needed.

---

## What You Need

- **ESP32-CAM-MB** (ESP32-CAM + USB dock board set)
- **MicroSD card** (FAT32 formatted, Class 10 or higher)
- **Micro USB cable**

> ESP32-CAM-MB is available on AliExpress or local electronics stores for around $3–5.

---

## Step 1: Download Firmware

Go to the [Releases](../../releases) page and download the latest `femtocam.bin` file.

---

## Step 2: Flash Firmware

### Option A: ESP Web Flasher (Easiest)

No software installation needed — flash directly from your browser.

1. Plug ESP32-CAM into the dock board
2. Connect to PC via USB
3. Open [ESP Web Flasher](https://esp.huhn.me/) in Chrome or Edge
4. Click **Connect** → select serial port (CH340 or COM port)
5. Set Address to `0x10000`
6. Select the downloaded `femtocam.bin`
7. Click **Program** → wait for completion

### Option B: ESP Flash Download Tool (Windows)

1. Download [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)
2. Select **ESP32** → **Develop**
3. Settings:
   - `femtocam.bin` → Address: `0x10000`
   - SPI SPEED: 40MHz / SPI MODE: DIO
   - Select COM port
4. Click **START**

---

## Step 3: Assemble

1. Insert a MicroSD card into the slot on the back of ESP32-CAM
2. Plug into the dock board and connect USB power — ready to use

> 💡 **Tip**: The dock board works fine for most users. If WiFi is unstable, try removing the dock and powering via the 5V/GND pins directly.

### Power Options

| Method | Notes |
|--------|-------|
| USB → dock board | Simplest. Works for most setups |
| 5V adapter → 5V/GND pins | Recommended if WiFi is unstable |
| Printer mainboard 5V | Powers on/off with the printer |

---

## Step 4: First Connection

1. Power on — the **red LED turns on** (AP mode)
2. On your phone, connect to WiFi **FEMTOCAM** (no password)
3. Open browser → `http://192.168.4.1/`
4. If you see the camera stream, you're done!

> If the LED blinks, it's trying to connect to a saved WiFi. If it stays on, it entered AP mode.

---

## Step 5: WiFi Setup (Optional)

Connect to your home/workshop WiFi so you can access from any device on the network.

1. Go to Settings tab → **Scan**
2. Select your network
3. Enter password
4. Click **Save & reboot**
5. After reboot, access via `http://femtocam.local/`

> If mDNS (`femtocam.local`) doesn't work, check your router's admin page for the IP assigned to ESP32-CAM.

---

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| LED blinks rapidly, won't stop | Camera init failed | Re-seat camera connector, reboot |
| FEMTOCAM WiFi not visible | Boot failure | Disconnect and reconnect power |
| Web page slow or frozen | Dock board connected | Remove dock, use separate 5V |
| Stream works but recording fails | SD card issue | Use FAT32 formatted Class 10+ card |
| VGA stream stutters | WiFi bandwidth | Switch to QVGA or move closer to router |

---

## Using Without SD Card

Streaming and settings work fine without an SD card. Only recording is disabled.

---

[← Back to README](../README.md)
