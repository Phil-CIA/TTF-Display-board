# Project Status

## Summary
Standalone ESP32-S3 display co-processor board with LVGL and multi-protocol command interface. Separated from the Development-Station-Power-Supply project to allow independent hardware and firmware iteration.

## Current State (2026-04-14)

### Origin
- Derived from the `front-display-board` in the Development-Station-Power-Supply repo
- Original board used an ESP32-C6-WROOM-1-N4 and drove the TFT directly via SPI
- Architecture revised to co-processor model: host sends commands, ESP32-S3 owns all rendering

### Hardware — Decisions Made
- **Rev 0 (archived):** ESP32-C6-WROOM-1-N4, no PSRAM, 10-pin IDC carrying raw TFT SPI signals
  - C6 board will **remain in service** in the power supply project (static screens, sufficient)
- **Next-iter (in design):** ESP32-S3-WROOM-1-**N16R8** selected (16MB flash, **8MB PSRAM**)
  - Dev board ordered: Hosyond 3-pack S3-N16R8 ~$18.99 (3-pack), arriving ~2026-04-17
  - PSRAM confirmed physically on N16R8 module — full LVGL frame buffer available
- **Display:** 4" Hoysond 480×320 ST7796S with XPT2046 resistive touch and SD card slot
  - TN panel retained for now — IPS/capacitive upgrade deferred until GUI is developed
  - Decision: prove the GUI first, then evaluate panel upgrade based on real results
- **Host interface:** 10-pin IDC repurposed to command protocol + USB-C for power/flash

### Hardware Limiting Factor Analysis (2026-04-14)
For "phone quality feel" the full chain must be addressed in order:
1. **Display bus** — SPI is the primary bottleneck (~10–20 fps full screen). ESP32-S3 parallel LCD interface can reach 60 fps at 480×320. Future rev target.
2. **Panel type** — TN (current) acceptable for instrument use. IPS needed for wide viewing angle and color accuracy. Deferred.
3. **Touch type** — Resistive XPT2046 (current) feels dated. Capacitive (GT911/CST816) is the single biggest "phone feel" upgrade. Deferred.
4. **MCU** — S3 at 240MHz with 8MB PSRAM is sufficient for smooth LVGL on SPI bus today.

### Firmware
- PlatformIO skeleton created — `firmware/src/main.cpp` Phase 1 boot + PSRAM check ready
- Target board: `esp32-s3-devkitc-1` (verify GPIO silk labels on Hosyond board on arrival)

### Software / LVGL
- LVGL selected as graphics engine — learning phase in progress
- With 8MB PSRAM: full 300KB frame buffer easily allocated, no compromise needed
- Without PSRAM (C6): 1/10 frame buffer (~15KB) works for static instrument screens

## Immediate Next Priorities
1. ✅ Archive Rev 0 KiCad files
2. ✅ Create next-iter KiCad working folder
3. ✅ Order ESP32-S3-N16R8 dev board
4. Get LVGL + ST7796S driver running on S3 devkit with the existing 4" TFT (Phase 1)
5. Implement UART command parser — label and bar updates from serial (Phase 2)
6. Add WiFi TCP command server (Phase 3)
7. Add SPI slave command receiver (Phase 4)
8. Begin next-iter schematic: swap C6 → S3-N16R8, repurpose IDC, add USB-C

## Open Questions
- Final IDC pinout — SPI slave only, or SPI + UART on same connector?
- Touch events: handle locally on display ESP32, or forward to host?
- Panel upgrade timing — IPS + capacitive once GUI is proven?
- MCU not limited to ESP32 long-term — revisit when phone-quality animation is targeted
