# TTF Display Board

ESP32-S3 co-processor display module with LVGL and a multi-protocol command interface.

## Concept

This board offloads all display rendering from the host MCU. The host sends simple text commands over SPI, UART, or WiFi. The display ESP32 owns all LVGL layout, animation, and widget management — the host never deals with pixels.

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
