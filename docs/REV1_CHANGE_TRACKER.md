# Rev 1 Change Tracker

Tracks all hardware changes from Rev 0 (ESP32-C6, Development-Station-Power-Supply) to Rev 1 (ESP32-S3, standalone TTF-Display-board).

## Status Key
- 🔴 Open — not started
- 🟡 In Progress
- 🟢 Resolved

---

## Hardware Changes

### HW-001 — Replace ESP32-C6 with ESP32-S3-N16R8
**Status:** 🟡 In Progress  
**Priority:** Critical  
**Reason:** ESP32-C6-WROOM-1-N4 has no PSRAM. LVGL requires ~300KB frame buffer for 480×320 16-bit color — beyond C6 SRAM headroom. ESP32-S3-N16R8 provides 8MB PSRAM and 16MB flash.  
**Decision (2026-04-14):** Upgraded target from N8R8 → **N16R8** (16MB flash, 8MB PSRAM). Dev board ordered: Hosyond 3-pack ~$6.33/each, arriving ~2026-04-17.  
**Note:** C6 board remains in service in the power supply project. Static instrument screens work fine on C6 with reduced LVGL draw buffer.  
**Action:** Replace `ESP32-C6-WROOM-1-N4` symbol and footprint with `ESP32-S3-WROOM-1-N16R8` in next-iter schematic. Verify GPIO assignments against Hosyond devkit silk labels on arrival.

---

### HW-002 — Repurpose 10-pin IDC to command protocol signals
**Status:** 🔴 Open  
**Priority:** High  
**Reason:** Original IDC carried raw TFT SPI signals to host. In co-processor architecture the host no longer drives the TFT — IDC should carry host command protocol signals (SPI slave + UART + handshake).  
**Action:** Reassign IDC pins per Architecture.md proposed pinout. Update schematic net labels.

---

### HW-003 — Add USB-C connector
**Status:** 🔴 Open  
**Priority:** High  
**Reason:** Board needs power and firmware flash/debug access independent of IDC. USB-C with CP2102N or CH343 provides USB-Serial bridge and 5V power input.  
**Action:** Add USB-C connector, ESD protection (USBLC6-2SC6), and USB-Serial bridge IC.

---

### HW-004 — Remove J1 (Back of TFT) and J2 (SD Connection) breakout connectors
**Status:** 🔴 Open  
**Priority:** Medium  
**Reason:** Rev 0 used these as external breakouts for the TFT module. Rev 1 integrates the TFT connector directly on the board without needing secondary breakouts.  
**Action:** Remove J1 and J2 from schematic. Route TFT signals directly to ESP32-S3 GPIOs.

---

### HW-005 — Review and finalize LVGL-friendly GPIO assignments
**Status:** 🔴 Open  
**Priority:** Medium  
**Reason:** ESP32-S3 has different GPIO constraints vs C6 (some pins input-only, internal pull restrictions). LVGL SPI needs DMA-capable pins.  
**Action:** Map TFT SPI, touch SPI, SD SPI, encoder, UART, and IDC signals to valid S3 GPIOs. Confirm no conflicts.

---

### HW-006 — Add 3V3 LDO or verify module regulator capacity
**Status:** 🔴 Open  
**Priority:** Low  
**Reason:** ESP32-S3 + TFT backlight + touch + SD + encoder peak current may exceed what the module's internal LDO can sustain from 5V IDC input.  
**Action:** Calculate peak current. Add external LDO (e.g. AMS1117-3.3 or AP2112K) if needed.

---

## Firmware / Software Changes

### FW-001 — Initialize PlatformIO project for ESP32-S3
**Status:** 🔴 Open  
**Action:** Create `platformio.ini` targeting `esp32-s3-devkitc-1`, configure PSRAM, set LVGL build flags.

---

### FW-002 — Integrate LVGL
**Status:** 🔴 Open  
**Action:** Add LVGL via PlatformIO library manager. Configure display driver for ST7796S. Verify frame buffer in PSRAM.

---

### FW-003 — Implement UART command parser
**Status:** 🔴 Open  
**Action:** Newline-delimited text command parser mapping to LVGL widget updates.

---

### FW-004 — Implement WiFi TCP command server
**Status:** 🔴 Open  
**Action:** SoftAP or station mode TCP server accepting same command protocol as UART.

---

### FW-005 — Implement SPI slave command receiver
**Status:** 🔴 Open  
**Action:** ESP32-S3 SPI slave mode receiving binary-framed command packets from host.

---

## Deferred / Future

### FUT-001 — Touch event forwarding to host
**Status:** 🔴 Open  
**Notes:** Decide whether touch events stay local (display acts autonomously) or are forwarded to host over UART/SPI. Defer until command protocol is stable.

### FUT-002 — Image/animation playback from SD card
**Status:** 🔴 Open  
**Notes:** SD card present on display module. LVGL can decode and display images. Useful for game-style animation sequences.

### FUT-003 — OTA firmware update over WiFi
**Status:** 🔴 Open  
**Notes:** ESP32-S3 supports OTA. Useful for bench-tool deployment without USB access.
