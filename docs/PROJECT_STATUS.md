# Project Status

## Summary
Standalone ESP32-S3 display co-processor board with LVGL and multi-protocol command interface. Separated from the Development-Station-Power-Supply project to allow independent hardware and firmware iteration.

## Current State (2026-04-14)

### Origin
- Derived from the `front-display-board` in the Development-Station-Power-Supply repo
- Original board used an ESP32-C6-WROOM-1-N4 and drove the TFT directly via SPI
- Architecture is being revised to co-processor model: host sends commands, ESP32-S3 owns all rendering

### Hardware
- Rev 0 (archived): ESP32-C6-WROOM-1-N4, no PSRAM, 10-pin IDC carrying raw TFT SPI signals
- Rev 1 (in design): ESP32-S3-WROOM-1-N8R8, 8MB PSRAM, repurposed IDC for command protocol
- Display: 4" Hoysond 480×320 ST7796S with XPT2046 touch and SD card slot
- Connector to host: 10-pin IDC (repurposed) + USB-C (power/flash)

### Firmware
- Not started yet — PlatformIO project to be initialized for ESP32-S3

### Software / LVGL
- LVGL selected as graphics engine — learning phase in progress

## Immediate Next Priorities
1. Archive Rev 0 KiCad files into `hardware/kicad/front-display-board-rev0-archive/`
2. Define Rev 1 hardware change list in `docs/REV1_CHANGE_TRACKER.md`
3. Finalize 10-pin IDC pinout for command protocol
4. Initialize PlatformIO firmware project targeting ESP32-S3
5. Get LVGL running with a basic screen on bench hardware (Phase 1)
6. Implement UART command parser (Phase 2)
7. Add WiFi TCP command server (Phase 3)
8. Add SPI slave command receiver (Phase 4)

## Open Questions
- Final IDC pinout — SPI slave only, or SPI + UART on same connector?
- Touch integration — pass touch events back to host, or handle locally on display ESP32?
- USB-C power only, or full USB-Serial bridge (CP2102/CH340)?
- Single-board product or carrier + module approach for rev 1 PCB?
