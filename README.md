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

## Hardware Summary

| Item | Choice |
|---|---|
| MCU | ESP32-S3-WROOM-1-N8R8 (8MB flash, 8MB PSRAM) |
| Display | 4" Hoysond 480×320 ST7796S SPI TFT |
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
│       ├── front-display-board-rev1/   # Active design (ESP32-S3 revision)
│       └── front-display-board-rev0-archive/  # Original ESP32-C6 board
└── assets/                      # Images, diagrams
```

## Related Projects

- [Development-Station-Power-Supply](https://github.com/Phil-CIA/Development-Station-Power-Supply) — power supply project this board is integrated into
