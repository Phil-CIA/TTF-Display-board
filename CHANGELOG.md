# Changelog

All notable changes to this project will be documented here.

## [Unreleased]

## [2026-04-23]

### Changed
- `README.md` — updated active hardware summary to reflect Elecrow CrowPanel Advance 4.3 as the current evaluation target
- `README.md` — corrected hardware directory structure (`front-display-board-next-iter` and `front-display-board-rev1` entries) and MCU listed as N16R8
- `firmware/platformio.ini` — corrected target comment from N8R8 to N16R8
- `firmware/src/main.cpp` — corrected target comment from N8R8 to N16R8
- `docs/ARCHITECTURE.md` — added on-hold status note directing to CrowPanel integration docs
- `docs/LVGL_NOTES.md` — added CrowPanel active-board callout; updated frame buffer calculation for 800×480; added ST7265 and SquareLine Studio notes
- `docs/DISPLAY_SIGNAL_VERIFICATION_WORKSHEET.md` — clarified scope as Hosyond MSP4022 / custom PCB path only, not CrowPanel
- `docs/boards/elecrow-crowpanel-advance-4.3/source-index/RESOURCE_INDEX.md` — expanded technical facts with confirmed LCD driver (ST7265), touch type (capacitive), I2C address (0x5D), STC8H1K28 I/O expander, UART1/MIC shared-function warning; added PlatformIO v1.1 and SquareLine Studio v1.1 tutorial links; updated mirroring backlog

## [2026-04-22]

### Added
- `docs/boards/elecrow-crowpanel-advance-4.3/README.md` — Elecrow board pack landing page
- `docs/boards/elecrow-crowpanel-advance-4.3/source-index/RESOURCE_INDEX.md` — official source registry and provenance
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_CONNECTION_STRATEGY.md` — recommended host integration approach
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_FIRST_POWERUP.md` — bench bring-up and failure-signature checklist
- `docs/boards/elecrow-crowpanel-advance-4.3/datasheets/README.md` — datasheet mirroring policy and targets
- `docs/boards/elecrow-crowpanel-advance-4.3/images/README.md` — revision photo capture guidance
- `docs/boards/elecrow-crowpanel-advance-4.3/datasheets/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf` — official Espressif ESP32-S3-WROOM-1/1U datasheet (mirrored)
- `docs/PROJECT_STATUS.md` — updated to reflect Elecrow CrowPanel evaluation track

## [2026-04-18]

### Added
- `docs/HOSYOND_MSP4022_DISPLAY_ISSUES.md` — working notes on Hosyond 4 inch TFT bring-up risk and controller ambiguity
- `docs/DISPLAY_SIGNAL_VERIFICATION_WORKSHEET.md` — code-versus-schematic verification worksheet for the PCB-mounted display path

## [2026-04-14]

### Added
- Initial repo scaffold for TTF Display Board project
- `docs/PROJECT_STATUS.md` — initial current state and immediate priorities
- `docs/ARCHITECTURE.md` — command protocol and system design reference
- `docs/REV1_CHANGE_TRACKER.md` — tracked hardware changes from Rev 0 (C6) to next-iter (S3)
- `docs/LVGL_NOTES.md` — LVGL learning notes and implementation reference
- `hardware/kicad/front-display-board-rev0-archive/` — original ESP32-C6 board files archived from Development-Station-Power-Supply repo
- `hardware/kicad/front-display-board-next-iter/` — working copy for S3-based redesign
- `firmware/` — PlatformIO project skeleton (ESP32-S3 target, PSRAM enabled)

### Decisions captured
- MCU upgrade target confirmed: ESP32-S3-WROOM-1-**N16R8** (16MB flash, 8MB PSRAM) — dev board ordered
- C6 board retained in power supply project for static instrument screens — no redesign needed there
- PSRAM requirement confirmed: 8MB needed for full LVGL frame buffer at 480×320 16-bit
- SPI bus identified as primary bottleneck for phone-quality animation — future parallel LCD interface planned
- Co-processor command model confirmed: host sends text commands, display ESP32 owns all LVGL rendering


