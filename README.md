# TTF Display Board

Reference-only concept repo for the earlier standalone display-board exploration.

## Status

This repo is currently **on hold**.

The active front-panel display work has moved back into the main Development Station Power Supply project. The current plan is:
- bring up the existing custom front-panel hardware already built
- evaluate an Elecrow CrowPanel Advance 4.3 inch HMI board as an off-the-shelf option
- avoid any new custom display redesign until there is bench evidence that it is needed

Elecrow board reference bundle:
- `docs/boards/elecrow-crowpanel-advance-4.3/README.md`

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
