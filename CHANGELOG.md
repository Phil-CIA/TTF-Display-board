# Changelog

All notable changes to this project will be documented here.

## [Unreleased]

### Added
- Initial repo scaffold for TTF Display Board project
- `docs/PROJECT_STATUS.md` — current state and immediate priorities
- `docs/ARCHITECTURE.md` — command protocol and system design reference
- `docs/REV1_CHANGE_TRACKER.md` — tracked hardware changes from Rev 0 (C6) to next-iter (S3)
- `docs/LVGL_NOTES.md` — LVGL learning notes and implementation reference
- `docs/HOSYOND_MSP4022_DISPLAY_ISSUES.md` — working notes on Hosyond 4 inch TFT bring-up risk and controller ambiguity
- `docs/DISPLAY_SIGNAL_VERIFICATION_WORKSHEET.md` — code-versus-schematic verification worksheet for the PCB-mounted display path
- `docs/boards/elecrow-crowpanel-advance-4.3/README.md` — Elecrow board pack landing page
- `docs/boards/elecrow-crowpanel-advance-4.3/source-index/RESOURCE_INDEX.md` — official source registry and provenance
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_CONNECTION_STRATEGY.md` — recommended host integration approach
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_FIRST_POWERUP.md` — bench bring-up and failure-signature checklist
- `docs/boards/elecrow-crowpanel-advance-4.3/datasheets/README.md` — datasheet mirroring policy and targets
- `docs/boards/elecrow-crowpanel-advance-4.3/images/README.md` — revision photo capture guidance
- `hardware/kicad/front-display-board-rev0-archive/` — original ESP32-C6 board files archived from Development-Station-Power-Supply repo
- `hardware/kicad/front-display-board-next-iter/` — working copy for S3-based redesign
- `firmware/` — PlatformIO project skeleton (ESP32-S3 target, PSRAM enabled)

### Decisions captured (2026-04-14)
- MCU upgrade target confirmed: ESP32-S3-WROOM-1-**N16R8** (16MB flash, 8MB PSRAM) — dev board ordered
- C6 board retained in power supply project for static instrument screens — no redesign needed there
- TN display panel retained for now — IPS/capacitive upgrade deferred until GUI proven
- PSRAM requirement confirmed: 8MB needed for full LVGL frame buffer at 480×320 16-bit
- SPI bus identified as primary bottleneck for phone-quality animation — future parallel LCD interface planned
- Co-processor command model confirmed: host sends text commands, display ESP32 owns all LVGL rendering

### Notes
- Board separated from Development-Station-Power-Supply repo to allow independent iteration
- Rev 0 board (ESP32-C6-WROOM-1-N4) remains in service in the power supply; next-iter is the standalone display co-processor
- Elecrow migration docs now use revision-gated bring-up guidance to prevent pin-map assumptions before board version identification
