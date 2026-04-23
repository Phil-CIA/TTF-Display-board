# Project Status

## Summary
Reference-only record of the standalone ESP32 display-board exploration. Active front-panel work has now moved back into the Development-Station-Power-Supply project.

## Current State (2026-04-15)

### Direction Change
- The separate standalone redesign is **paused / on hold**
- No immediate custom display-board redesign is planned
- The display effort has pivoted back into the main power-supply repo
- Two active paths remain:
  1. Bring up the existing custom front-panel board already built
  2. Evaluate the Elecrow CrowPanel Advance 4.3 inch HMI board as a faster off-the-shelf UI option

### Origin
- Derived from the `front-display-board` in the Development-Station-Power-Supply repo
- Original board used an ESP32-C6-WROOM-1-N4 and drove the TFT directly via SPI
- This repo captured the earlier idea of a separate display co-processor board

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
- Reference bring-up docs now include a Hosyond display issues note and a signal-verification worksheet for checking PCB labels against firmware pin assignments

### Elecrow Documentation Track (2026-04-22)
- Added a dedicated Elecrow board documentation bundle under `docs/boards/elecrow-crowpanel-advance-4.3/`
- Added source provenance index for official Elecrow wiki/repo links and integration facts
- Added a connection strategy that treats CrowPanel as a self-hosted HMI node linked to host via UART-first protocol
- Added a first power-up checklist with revision-identification gate before fixed pin assumptions

### HMI Docs Aligned to DSPS Channel Model (2026-04-23)
- Added `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md` as canonical UI capability source, derived from Development-Station-Power-Supply `src/main.cpp` and `HANDOFF.md`
- Rail model confirmed: Rail5V (fixed 5V), Rail3V3 (fixed 3.3V), RailAdj (discrete relay 5V/3.3V select), RailIncoming12V (monitor only)
- Added `ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md` with state machine, UART commands, and relay-safe interlock rule for RailAdj
- Added `ELECROW_CROWPANEL_43_SCREEN_MAP.md` with 5-screen layout; no analog sliders; RailAdj uses 2-position selector grayed when rail is on
- Verified no variable-setpoint wording exists in any Elecrow doc; all setpoint-related lines are explicit prohibitions

### Software / LVGL
- LVGL selected as graphics engine — learning phase in progress
- With 8MB PSRAM: full 300KB frame buffer easily allocated, no compromise needed
- Without PSRAM (C6): 1/10 frame buffer (~15KB) works for static instrument screens

## Immediate Next Priorities
1. Bring up the existing front-panel hardware in the main power-supply project
2. Execute the Elecrow first power-up checklist and capture board revision photos in `docs/boards/elecrow-crowpanel-advance-4.3/images/`
3. Bench-test the Elecrow CrowPanel Advance 4.3 inch board with USB-only bring-up first, then UART host link
4. Define and validate a simple common host-to-display interface, likely 5V + GND + UART TX/RX
5. Keep this repo only as design reference unless a future redesign becomes justified by testing

## Open Questions
- Which path proves most practical on the bench: existing custom board or Elecrow board?
- What minimal command set should the host expose so either display can be used?
- Is any future custom redesign still needed after real bring-up data is collected?

## New Reference Docs
- `docs/boards/elecrow-crowpanel-advance-4.3/README.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/source-index/RESOURCE_INDEX.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_CONNECTION_STRATEGY.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_FIRST_POWERUP.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_SCREEN_MAP.md`
