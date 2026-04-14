# Changelog

All notable changes to this project will be documented here.

## [Unreleased]

### Added
- Initial repo scaffold for TTF Display Board project
- `docs/PROJECT_STATUS.md` — current state and immediate priorities
- `docs/ARCHITECTURE.md` — command protocol and system design reference
- `docs/REV1_CHANGE_TRACKER.md` — tracked hardware changes from Rev 0 (C6) to Rev 1 (S3)
- `docs/LVGL_NOTES.md` — LVGL learning notes and implementation reference
- `hardware/kicad/front-display-board-rev0-archive/` — original ESP32-C6 board files copied from Development-Station-Power-Supply repo
- `hardware/kicad/front-display-board-rev1/` — target folder for S3-based redesign
- `firmware/` — PlatformIO project skeleton (ESP32-S3 target)

### Notes
- Board separated from Development-Station-Power-Supply repo to allow independent iteration
- Rev 0 board (ESP32-C6-WROOM-1-N4) will remain in service in the power supply; Rev 1 is the standalone display co-processor
