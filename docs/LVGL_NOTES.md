# LVGL Notes

Running notes and reference material as we learn and implement LVGL on this project.

> **Active board:** Elecrow CrowPanel Advance 4.3 (v1.1) — ESP32-S3-WROOM-1-N16R8, 800×480 IPS, LCD driver ST7265, capacitive touch via I2C.
> The Hosyond MSP4022 / ST7796S references below apply to the custom board exploration path (on hold).

## What LVGL Is

LVGL (Light and Versatile Graphics Library) is an embedded GUI framework. It manages:
- Screens (full pages)
- Objects (labels, buttons, bars, charts, images, etc.) placed on screens
- Styles (CSS-like reusable visual properties)
- Animations
- An internal task scheduler for smooth updates

It runs entirely on the ESP32 and drives the TFT through a display driver.

## Frame Buffer and PSRAM

LVGL needs at least one draw buffer. For 800×480 at 16-bit color (CrowPanel):
- Full frame buffer = 800 × 480 × 2 = **768,000 bytes (~750KB)**
- ESP32-S3-N16R8 PSRAM = 8MB — place frame buffer in PSRAM, no problem

For 480×320 at 16-bit color (Hosyond custom board path):
- Full frame buffer = 480 × 320 × 2 = **307,200 bytes (~300KB)**
- ESP32-C6 total SRAM = 512KB — not viable with application code
- ESP32-S3-N16R8 PSRAM = 8MB — place frame buffer in PSRAM, no problem

```c
// platformio.ini build flags needed
build_flags =
  -DBOARD_HAS_PSRAM
  -mfix-esp32-psram-cache-issue
  -DLV_MEM_SIZE=512*1024   ; LVGL heap in PSRAM (sufficient for 800x480 full buffer)
```

## Display Driver

**For Elecrow CrowPanel Advance 4.3 (ST7265, 800×480):**
- LCD driver IC is **ST7265** — check Elecrow vendor examples for the correct init sequence
- Vendor supports Arduino IDE + LVGL; examples at https://github.com/Elecrow-RD/CrowPanel-Advance-4.3--HMI-ESP32-800x480
- **SquareLine Studio** is the recommended UI design tool for this board (vendor-supported workflow)
- LovyanGFX also supports the CrowPanel family; confirm the exact board config from vendor examples

**For custom board (ST7796S, 480×320, on hold):**
- For ST7796S with LVGL, the `lv_driver` or `arduino-gfx` + `lvgl` combination works well.
- TFT_eSPI is also widely used and has ST7796 support.

Recommended approach:
- **LovyanGFX** as the display driver (fast, DMA, good S3 support)
- **LVGL 8.x or 9.x** linked against LovyanGFX

## Key LVGL Objects

| Object | LVGL type | Command trigger |
|---|---|---|
| Text display | `lv_label_t` | `CMD:LABEL` |
| Progress bar | `lv_bar_t` | `CMD:BAR` |
| Gauge/arc | `lv_arc_t` | `CMD:ARC` |
| Line chart | `lv_chart_t` | `CMD:CHART` |
| Image | `lv_img_t` | `CMD:IMG` |
| Screen | `lv_obj_t` | `CMD:SCREEN` |

## Useful References

- LVGL docs: https://docs.lvgl.io
- LovyanGFX: https://github.com/lovyan03/LovyanGFX
- PlatformIO LVGL: https://registry.platformio.org/libraries/lvgl/lvgl
- ST7796S datasheet: search "ST7796S datasheet Sitronix"

## Phase 1 Goal

Get LVGL running with a label on screen:
```c
lv_obj_t *scr = lv_scr_act();
lv_obj_t *label = lv_label_create(scr);
lv_label_set_text(label, "TTF Display Ready");
lv_obj_center(label);
```

## Notes Log

*(Add bench observations here as work progresses)*
