# TTF Display Board


**Purpose:** Develop a HMI display interface for the [Development-Station-Power-Supply](https://github.com/Phil-CIA/Development-Station-Power-Supply) PSU.

## Status (2026-04-23)

**Active path:** CrowPanel Advance 4.3 HMI integration  
**Status:** ✅ UART protocol spec, PSU firmware stubs, CrowPanel firmware scaffold, phased bring-up plan complete

See [`docs/PROJECT_STATUS.md`](docs/PROJECT_STATUS.md) for detailed progress.

## Two Paths Within This Repo

### Path 1: Elecrow CrowPanel Advance 4.3 (ACTIVE)
- **Purpose:** Off-the-shelf 4.3" 800×480 HMI board linked to PSU via UART
- **Status:** Firmware scaffold complete, ready for SquareLine display driver integration and Phase A bench bring-up
- **Firmware:** `firmware/` — ESP32-S3 (PSRAM), LVGL 9.2, FreeRTOS UART tasks, 5-screen manager
- **Docs:** `docs/boards/elecrow-crowpanel-advance-4.3/` — capability profile, UART protocol, operations contract, screen map, bring-up plan
- **Next steps:** SquareLine studio integration, GPIO pin assignment, phase bring-up validation

### Path 2: front-display-board-rev0 (ARCHIVED VARIANT)
- **Purpose:** Custom ESP32-S3 display co-processor with TFT driver (effort/conversion consideration)
- **Status:** Archived but valid — could be developed in parallel or converted to CrowPanel architecture
- **Hardware:** `hardware/kicad/front-display-board-rev0-archive/` — ESP32-C6 or S3, 4" TFT with touch
- **Scope:** Separate effort; will be aligned to CrowPanel approach or used as reference for future custom redesign

## Integration with Development-Station-Power-Supply

Both display paths connect to the PSU via the same UART protocol:
- **PSU firmware** ([Development-Station-Power-Supply](https://github.com/Phil-CIA/Development-Station-Power-Supply), commit `900541a`): `CMD:` HMI UART handler in `handleSerialCommands()`
- **HMI firmware** (this repo, commit `03a500d`+): async UART task layer parsing `ACK:` / `ERR:` / `EVT:` frames
- **Protocol** (ELECROW_CROWPANEL_43_UART_PROTOCOL.md): Formal spec with 6 commands, error codes, sequence diagrams

CrowPanel board reference bundle:
- [`docs/boards/elecrow-crowpanel-advance-4.3/README.md`](docs/boards/elecrow-crowpanel-advance-4.3/README.md)
## Original concept

This repo explored an ESP32-S3 co-processor display module with LVGL and a multi-protocol command interface. That architectural idea is still useful as reference, but it is not the active hardware path right now.

```
HOST MCU or PC
     │
     │  SPI / UART / WiFi
     │
┌────▼────────────────────────┐
│  ESP32-S3 (PSRAM)           │
│  Command Parser             │
│  LVGL Engine                │
│         │                   │
│    SPI (internal fast bus)  │
│         │                   │
│  ST7796S TFT Driver         │
└─────────┴───────────────────┘
          │
    4" 480×320 Color TFT
    (touch optional)
```

## Active Evaluation Hardware — Elecrow CrowPanel Advance 4.3

| Item | Detail |
|---|---|
| Board | Elecrow CrowPanel Advance 4.3 (model DIS01943A), revision v1.1 |
| MCU | ESP32-S3-WROOM-1-N16R8 (16MB flash, 8MB PSRAM) |
| Display | 4.3" IPS 800×480, LCD driver IC ST7265 |
| Touch | Capacitive, I2C at 0x5D (via STC8H1K28 I/O expander) |
| Host interface | UART0-IN (IO44/IO43) — 5V power + UART |
| I2C | SDA(IO15), SCL(IO16) |
| Graphics | LVGL (built-in demo, SquareLine Studio supported) |
| Power | DC 5V 2A external, or USB-C |

See [`docs/boards/elecrow-crowpanel-advance-4.3/`](docs/boards/elecrow-crowpanel-advance-4.3/) for integration guidance, bring-up checklist, and source index.

## Original Custom Board Concept (On Hold)

The original concept explored here was a custom ESP32-S3 co-processor display module with LVGL and a multi-protocol command interface. That architectural idea remains useful as reference, but it is not the active hardware path right now.

| Item | Choice |
|---|---|
| MCU | ESP32-S3-WROOM-1-N16R8 (16MB flash, 8MB PSRAM) |
| Display | 4" Hosyond 480×320 ST7796S SPI TFT |
| Touch | XPT2046 (on display module) |
| SD card | On display module, shared SPI bus |
| User input | Rotary encoder with pushbutton |
| Host interface | 10-pin IDC (SPI + UART + handshake) |
| Power/Flash | USB-C (power + serial) |
| Graphics | LVGL |

## Repository Structure

```
TTF-Display-board/
├── docs/                        # Design docs, planning, change tracker
│   └── boards/
│       └── elecrow-crowpanel-advance-4.3/  # Board-specific source index and bring-up guides
├── firmware/
│   ├── src/                     # PlatformIO source files
│   └── include/                 # Headers
├── hardware/
│   └── kicad/
│       ├── front-display-board-next-iter/  # Active custom design (ESP32-S3 revision, on hold)
│       ├── front-display-board-rev1/       # Earlier custom board checkpoint
│       └── front-display-board-rev0-archive/  # Original ESP32-C6 board
└── assets/                      # Images, diagrams
```

## Related Projects

- [Development-Station-Power-Supply](https://github.com/Phil-CIA/Development-Station-Power-Supply) — power supply project this board is integrated into
