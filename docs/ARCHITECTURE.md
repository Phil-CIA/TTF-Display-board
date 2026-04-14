# Architecture Reference

## System Concept

The TTF Display Board is a display co-processor. The host MCU or PC sends commands over a wire or wireless protocol. The display ESP32-S3 owns all LVGL layout, rendering, and animation. The host never deals with pixels, fonts, or display timing.

```
HOST (any MCU or PC)
        │
   SPI slave  /  UART  /  WiFi-TCP
        │
  ┌─────▼──────────────────────┐
  │  ESP32-S3 + 8MB PSRAM      │
  │  ┌─────────────────────┐   │
  │  │ Protocol Receivers  │   │
  │  │  SPI-S │ UART │ WiFi│   │
  │  └────────┬────────────┘   │
  │           │                │
  │  ┌────────▼────────────┐   │
  │  │ Command Dispatcher  │   │
  │  └────────┬────────────┘   │
  │           │                │
  │  ┌────────▼────────────┐   │
  │  │  LVGL Engine        │   │
  │  │  Screens / Widgets  │   │
  │  └────────┬────────────┘   │
  │           │ SPI (fast)     │
  │  ┌────────▼────────────┐   │
  │  │  ST7796S TFT Driver │   │
  └──┴─────────────────────┴───┘
               │
         4" 480×320 TFT
```

## Command Protocol

The host sends human-readable text commands. The display ESP32 parses and acts on them via LVGL.

### Command format
```
CMD:<action> [key=value ...]
```

### Core commands (planned)
| Command | Description |
|---|---|
| `CMD:LABEL id=<n> text="..." [x=n] [y=n] [color=n] [size=n]` | Create or update a text label |
| `CMD:BAR id=<n> val=<n> max=<n>` | Set a progress bar or meter value |
| `CMD:SCREEN name=<name>` | Switch to a named screen with optional animation |
| `CMD:CLEAR` | Clear current screen |
| `CMD:ALERT text="..." timeout=<ms>` | Show a temporary overlay alert |
| `CMD:IMG id=<n> src=<filename>` | Display an image from SD card |
| `CMD:ACK` | Host polls for ready/busy state |

### Protocol transport mapping
| Transport | Format | Use case |
|---|---|---|
| UART | Text (newline-delimited) | Simple MCU → display wired link |
| SPI slave | Binary-framed packet | Fast tight integration on-board |
| WiFi TCP | Text (newline-delimited) | Bench tool, PC → display, wireless |

## LVGL Architecture

LVGL organizes the display as:
- **Screens** — full display pages (`lv_scr_load_anim` for transitions)
- **Objects** — labels, bars, charts, buttons, images placed on screens
- **Styles** — reusable visual properties (color, font, padding, border)
- **Tasks** — LVGL internal scheduler for animations and timed updates

The display firmware defines all layout. The host only updates values and triggers screen changes.

## 10-Pin IDC Connector Pinout (proposed Rev 1)

| Pin | Signal | Direction | Notes |
|---|---|---|---|
| 1 | GND | — | |
| 2 | +5V | In | Host supplies power |
| 3 | SPI-MOSI | In | Host → display |
| 4 | SPI-MISO | Out | Display → host (ACK data) |
| 5 | SPI-SCK | In | SPI clock |
| 6 | SPI-CS | In | Chip select |
| 7 | UART-TX | Out | Display → host debug/event |
| 8 | UART-RX | In | Host → display commands |
| 9 | HANDSHAKE-A (BUSY) | Out | Display busy signal |
| 10 | HANDSHAKE-B (READY) | In | Host ready / trigger |

> This pinout is proposed. Finalization pending design review.

## Hardware Decisions Log

| Decision | Choice | Reason |
|---|---|---|
| MCU | ESP32-S3-WROOM-1-N8R8 | PSRAM required for LVGL frame buffer; C6 has no PSRAM |
| Display | ST7796S 480×320 | LVGL driver available, good SPI speed |
| Graphics | LVGL | Mature widget system, ESP32 support, learning opportunity |
| Host protocol | Command text over UART/SPI/WiFi | Host decoupled from pixel management |
| IDC repurpose | Command protocol signals | Old role (raw TFT SPI) no longer needed |
| USB-C | Power + USB-Serial | Flash, debug, bench power from single connector |
