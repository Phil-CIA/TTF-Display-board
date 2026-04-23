# Project Status


## Summary
Developing a UART-linked HMI display interface for the [Development-Station-Power-Supply](https://github.com/Phil-CIA/Development-Station-Power-Supply) PSU. Two board paths are active: (1) Elecrow CrowPanel 4.3" (primary development), (2) custom front-display-board-rev0 archived variant (parallel/conversion consideration).

## Current State (2026-04-23)

### Status: CrowPanel Path Complete & Ready for Bring-Up
- ✅ UART protocol formal spec (CMD:/ACK:/ERR:/EVT: frame format, 6 commands)
- ✅ PSU firmware stubs (`handleHmiCommand()` in Development-Station-Power-Supply commit `900541a`)
- ✅ CrowPanel firmware scaffold (LVGL 9.2, FreeRTOS UART tasks, 5-screen manager, commit `03a500d`)
- ✅ Phased bring-up plan (6 phases: loopback → fault recovery)
- **Next:** SquareLine studio for display driver + touch integration, Phase A bench validation

### Two Development Paths
1. **Elecrow CrowPanel Advance 4.3** (ACTIVE) — off-the-shelf 800×480 HMI, UART-linked
2. **front-display-board-rev0** (ARCHIVED VARIANT) — custom ESP32 board, still valid for parallel effort or conversion
### Origin

### Project Origins
- **Primary repo:** Development-Station-Power-Supply (PSU board/firmware) — [https://github.com/Phil-CIA/Development-Station-Power-Supply](https://github.com/Phil-CIA/Development-Station-Power-Supply)
- **This repo (TTF-Display-board):** Dedicated HMI display development for the PSU
- **Path 1 origin:** Elecrow CrowPanel Advance 4.3 — purchased as off-the-shelf HMI option, being integrated via UART
- **Path 2 origin:** front-display-board-rev0 (archived in `hardware/kicad/`) — custom ESP32-C6 board from earlier exploration, archived but still valid variant
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

1. **SquareLine studio integration** — generate display driver code for ST7265 + capacitive touch
2. **Phase A bench validation** — UART loopback test with PSU (cmd echo via debug terminal)
3. **Phase B boot test** — CrowPanel firmware boots, establishes link to PSU, periodic polling
4. **Phase C single-rail test** — end-to-end control: CMD:RAIL_ENABLE → voltage/current feedback
5. **Phase D live dashboard** — display renders, multi-rail control, responsiveness validation
6. **Phase E fault recovery** — fault injection, Fault Lock screen, recovery path
7. **front-display-board-rev0 decision** — after CrowPanel bring-up, decide: parallel effort, conversion, or archive-only reference
## Open Questions

- **Post bring-up:** After CrowPanel validates end-to-end, does front-display-board-rev0 get developed in parallel or converted to use CrowPanel design?
- **Custom redesign:** Is future custom ESP32 display board needed, or is CrowPanel sufficient long-term?
- **GPIO assignment:** Which ESP32 pins will be assigned for rail enable and RailAdj relay select on PSU side?
## New Reference Docs
- `docs/boards/elecrow-crowpanel-advance-4.3/README.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/source-index/RESOURCE_INDEX.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_CONNECTION_STRATEGY.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_FIRST_POWERUP.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md`
- `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_SCREEN_MAP.md`

### UART Protocol Formal Spec Added (2026-04-23)

New doc: `docs/boards/elecrow-crowpanel-advance-4.3/ELECROW_CROWPANEL_43_UART_PROTOCOL.md`

- Frame format: `CMD:<ACTION> key=value …\r\n` / `ACK:` / `ERR:` / `EVT:`
- Commands: READ_STATUS, RAIL_ENABLE, RAIL_VOLT_SEL, SET_MODE, RESET, ABORT
- Unsolicited events: EVT:FAULT, EVT:WATCHDOG
- Rail ID canonical tokens confirmed against DSPS firmware parser
- PSU integration approach: `CMD:` prefix check added at top of `handleSerialCommands()`, dispatches to new `handleHmiCommand()` — existing raw commands unchanged
- CrowPanel integration: uart_rx_task + uart_poll_task skeleton documented

### HMI Firmware Scaffold Complete (2026-04-23)

Repo: `TTF-Display-board/firmware/` (commit `03a500d`)

- **platformio.ini**: Updated for ESP32-S3 OPI PSRAM, LVGL 9.2 + LovyanGFX, -DDISPLAY_WIDTH=800 -DDISPLAY_HEIGHT=480
- **include/hmi_state.h**: HmiState (SafeIdle/Standby/ActiveRun/FaultLock), ScreenId, RailStatus structs, PendingCommand model
- **include/uart_link.h**: Async UART API (uart_rx_task, uart_poll_task, callback registration)
- **include/lv_conf.h**: LVGL 800×480 config with PSRAM OPI allocator, dark theme colors
- **include/screen_manager.h**: 5-screen manager declarations
- **src/main.cpp**: setup() integrates LVGL + UART + FreeRTOS tasks; loop() diagnostics heartbeat
- **src/uart_link.cpp**: uart_rx_task (core 0) parses ACK:/ERR:/EVT:, uart_poll_task (core 1) sends READ_STATUS every 500ms + 1500ms watchdog
- **src/screen_manager.cpp**: 5-screen skeleton (Dashboard, RailDetail, FaultLock, TrendView, ServiceMenu) with placeholder layouts

Status: Compiles cleanly (first LVGL build ~30s).
TODO: SquareLine studio integration for display driver + touch input, chart widget, button handlers

### Phased Bring-Up Plan (2026-04-23)

Docs: `BRING_UP_PLAN.md` in elecrow-crowpanel-advance-4.3 folder

- **Phase A**: UART loopback (PSU debug terminal, CMD: echo)
- **Phase B**: CrowPanel boot + link establish (periodic READ_STATUS polling)
- **Phase C**: Single rail enable + telemetry stream (voltage/current feedback)
- **Phase D**: Full dashboard with display + touch (multi-rail control, no lag)
- **Phase E**: Fault injection + recovery (Fault Lock screen, RESET hold)
- **Phase F**: Trend view + service menu (chart widget, diagnostics)

Includes setup, test sequences, success criteria, failure modes, debug procedures per phase.
