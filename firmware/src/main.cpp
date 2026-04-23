/**
 * main.cpp — CrowPanel Advance 4.3 HMI firmware
 * 
 * ESP32-S3 (16MB flash, 8MB PSRAM) + ST7265 800×480 display
 * UART0-IN (IO43 TX, IO44 RX) linked to PSU HAT via CMD:/ACK:/ERR:/EVT: protocol
 * 
 * Subsystems:
 *  - LVGL display rendering (lv_timer driven by esp_timer)
 *  - uart_rx_task (core 0): line-by-line UART parser
 *  - uart_poll_task (core 1): periodic READ_STATUS sender, watchdog
 *  - screen_manager: 5-screen state machine
 */
#include <Arduino.h>
#include <lvgl.h>
#include <esp_timer.h>
#include <esp_heap_caps.h>
#include "hmi_state.h"
#include "uart_link.h"

// Forward declarations (defined in separate source files)
extern void screen_manager_init();
extern void screen_manager_load(ScreenId id);
extern ScreenId screen_manager_get_active();

// ============================================================================
// LVGL tick provider (esp_timer based)
// ============================================================================

static void lv_tick_timer_cb(void* arg) {
    lv_tick_inc(1);
}

static void setup_lv_tick() {
    const esp_timer_create_args_t timer_args = {
        .callback = lv_tick_timer_cb,
        .name = "lv_tick",
    };
    esp_timer_handle_t timer = nullptr;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, 1 * 1000);  // 1ms tick
}

// ============================================================================
// LVGL render task (core 0, high priority)
// ============================================================================

static void lvgl_task(void* arg) {
    while (true) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// ============================================================================
// Arduino setup()
// ============================================================================

void setup() {
    // Serial console (USB CDC)
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n===== CrowPanel Advance 4.3 HMI Firmware =====");
    Serial.printf("ESP32-S3 @ %d MHz\n", getCpuFreqMHz());
    
    // PSRAM check
    if (psramFound()) {
        Serial.printf("PSRAM: %.1f MB available\n", ESP.getFreePsram() / 1048576.0f);
    } else {
        Serial.println("WARNING: PSRAM not detected!");
    }
    
    // Initialize LVGL
    Serial.println("[Init] LVGL...");
    lv_init();
    
    // Setup LVGL tick provider (esp_timer, 1ms)
    setup_lv_tick();
    
    // Create draw buffers in PSRAM (1/10 of display area)
    const size_t buf_size = (800 * 480 * LV_COLOR_DEPTH / 8) / 10;
    lv_color_t* buf1 = static_cast<lv_color_t*>(heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM));
    lv_color_t* buf2 = static_cast<lv_color_t*>(heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM));
    
    if (!buf1 || !buf2) {
        Serial.println("ERROR: Failed to allocate LVGL draw buffers!");
        while (true) { delay(1000); }
    }
    
    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (buf_size * 8) / LV_COLOR_DEPTH);
    
    // Display driver (dummy for now — will be configured via SquareLine studio)
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    // TODO: configure disp_drv.flush_cb to write to ST7265 via LovyanGFX
    lv_disp_drv_register(&disp_drv);
    
    // Input driver (dummy for now — will be configured for capacitive touch)
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    // TODO: configure indev_drv.read_cb to read from touch controller
    // lv_indev_drv_register(&indev_drv);
    
    Serial.println("[Init] LVGL OK");
    
    // Start UART subsystem (both tasks)
    Serial.println("[Init] UART RX task...");
    uart_rx_task_start();
    
    Serial.println("[Init] UART poll task...");
    uart_poll_task_start();
    
    // Initialize screen manager (creates all 5 screens, loads Dashboard)
    Serial.println("[Init] Screen manager...");
    screen_manager_init();
    
    // Start LVGL render task on core 0 (high priority, runs continuously)
    Serial.println("[Init] LVGL render task...");
    xTaskCreatePinnedToCore(lvgl_task, "lvgl", 8192, nullptr, configMAX_PRIORITIES - 1, nullptr, 0);
    
    Serial.println("[Boot] All systems online");
    Serial.println("Waiting for PSU link establishment...");
}

// ============================================================================
// Arduino loop() — minimal, most work in FreeRTOS tasks
// ============================================================================

static uint32_t lastHeartbeatMs = 0;

void loop() {
    // Periodic diagnostics
    uint32_t now = millis();
    if (now - lastHeartbeatMs >= 5000) {
        lastHeartbeatMs = now;
        
        const PsuStatus& psu = uart_get_psu_status();
        Serial.printf("[HB] state=%d mode=%s link=%s err=%s\n",
                      static_cast<int>(psu.state),
                      psu.mode,
                      psu.linkLost ? "LOST" : "OK",
                      psu.lastError);
        
        // Display heap stats
        uint32_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        uint32_t psram_used = heap_caps_get_total_size(MALLOC_CAP_SPIRAM) - psram_free;
        Serial.printf("    PSRAM: %u / %u KB used\n", psram_used / 1024, 
                      heap_caps_get_total_size(MALLOC_CAP_SPIRAM) / 1024);
    }
    
    delay(100);
}

