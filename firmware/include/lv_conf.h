/**
 * lv_conf.h — LVGL configuration for CrowPanel Advance 4.3
 * 800×480, ESP32-S3, 8MB PSRAM available
 *
 * Canonical reference: ELECROW_CROWPANEL_43_SCREEN_MAP.md
 */
#if 1  /* Set to 1 to enable content */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Color depth: 16 (RGB565) matches ST7265 native format */
#define LV_COLOR_DEPTH 16

/* Memory: use PSRAM via custom allocator */
#define LV_MEM_CUSTOM 1
#if LV_MEM_CUSTOM
#  define LV_MEM_CUSTOM_INCLUDE <esp_heap_caps.h>
#  define LV_MEM_CUSTOM_ALLOC(size)      heap_caps_malloc((size), MALLOC_CAP_SPIRAM)
#  define LV_MEM_CUSTOM_FREE(ptr)        free(ptr)
#  define LV_MEM_CUSTOM_REALLOC(ptr, s)  heap_caps_realloc((ptr), (s), MALLOC_CAP_SPIRAM)
#endif

/* Display resolution */
#define LV_HOR_RES_MAX 800
#define LV_VER_RES_MAX 480

/* Draw buffer: 1/10 of screen in PSRAM is sufficient */
#define LV_DRAW_BUF_ALIGN  4
#define LV_DRAW_BUF_STRIDE_ALIGN  1

/* Tick: provided by esp_timer in main.cpp */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
#  define LV_TICK_CUSTOM_INCLUDE <esp_timer.h>
#  define LV_TICK_CUSTOM_SYS_TIME_EXPR ((uint32_t)(esp_timer_get_time() / 1000ULL))
#endif

/* Font: built-in Montserrat sizes used by screen map */
#define LV_FONT_MONTSERRAT_12  1
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_DEFAULT        &lv_font_montserrat_14

/* Enable chart widget for Trend View screen */
#define LV_USE_CHART  1

/* Enable label, button, switch, arc, table — all used by screen map */
#define LV_USE_LABEL   1
#define LV_USE_BTN     1
#define LV_USE_SWITCH  1
#define LV_USE_ARC     1
#define LV_USE_TABLE   1
#define LV_USE_TABVIEW 1
#define LV_USE_BAR     1
#define LV_USE_MSGBOX  1
#define LV_USE_SPAN    1

/* Logging — route to Serial */
#define LV_USE_LOG  1
#if LV_USE_LOG
#  define LV_LOG_LEVEL  LV_LOG_LEVEL_WARN
#  define LV_LOG_PRINTF 1
#endif

/* Theme: dark */
#define LV_USE_THEME_DEFAULT  1
#define LV_THEME_DEFAULT_DARK 1

/* Screen map color tokens (used by screen_manager.cpp) */
#define LV_COLOR_BACKGROUND  0x1A1A2E
#define LV_COLOR_OK          0x00AA00
#define LV_COLOR_WARN        0xFFAA00
#define LV_COLOR_FAULT       0xFF3333
#define LV_COLOR_DISABLED    0xAAAAAA

#endif /* LV_CONF_H */
#endif /* Enable content */
