/**
 * src/screen_manager.cpp — 5-screen LVGL state machine
 *
 * Screens (canonical from ELECROW_CROWPANEL_43_SCREEN_MAP.md):
 *  0. Dashboard    — Safe Idle/Standby/Active Run badge, per-rail toggle, RailAdj [5V|3V3]
 *  1. RailDetail   — Selected rail detail, CL params, volt selector, enable/disable
 *  2. FaultLock    — Red alert, fault code, 3s hold RESET button
 *  3. TrendView    — 30s history (4V traces + 3I traces, LVGL chart)
 *  4. ServiceMenu  — CL config, mode, fault history, UART diagnostics
 *
 * Navigation: Dashboard is default; other screens driven by user taps or state.
 */
#include <Arduino.h>
#include <lvgl.h>
#include "hmi_state.h"
#include "uart_link.h"
#include <cstdio>

// Forward declarations
static void create_screen_dashboard();
static void create_screen_rail_detail();
static void create_screen_fault_lock();
static void create_screen_trend_view();
static void create_screen_service_menu();

static lv_obj_t* screens[5] = {nullptr};
static ScreenId activeScreen = ScreenId::Dashboard;

// Callbacks from UART task update PSU state, trigger screen redraws
static void on_psu_status_update(const PsuStatus& status);
static void on_psu_fault(const char* faultCode, const RailStatus& rail);
static void on_psu_error(const char* error);

// ============================================================================
// Stub implementations — full UI layout deferred to SquareLine studio
// ============================================================================

static void create_screen_dashboard() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    screens[static_cast<int>(ScreenId::Dashboard)] = screen;
    
    // Background
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1A1A2E), 0);
    lv_obj_set_size(screen, 800, 480);
    
    // Title bar: "Dashboard"
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Dashboard");
    lv_obj_set_pos(title, 20, 20);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    
    // Placeholder: per-rail status cards (Rails 0, 1, 2) + Rail 12V monitor
    // Each rail card has: name badge, voltage, current, ON/OFF toggle
    // RailAdj card also has: [5V] [3V3] exclusive selector
    
    lv_obj_t* placeholder = lv_label_create(screen);
    lv_label_set_text(placeholder, "Dashboard\n\nRail0: 5.00V / 0.00A [OFF]\nRail1: 3.30V / 0.00A [OFF]\nRail2: 0.00V / 0.00A [OFF] [5V|3V3]\nIn12V: 12.00V / 0.00A (read-only)");
    lv_obj_set_pos(placeholder, 40, 80);
    lv_obj_set_style_text_font(placeholder, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(placeholder, lv_color_hex(0xAAAAAA), 0);
}

static void create_screen_rail_detail() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    screens[static_cast<int>(ScreenId::RailDetail)] = screen;
    
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1A1A2E), 0);
    lv_obj_set_size(screen, 800, 480);
    
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Rail Detail");
    lv_obj_set_pos(title, 20, 20);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    
    lv_obj_t* placeholder = lv_label_create(screen);
    lv_label_set_text(placeholder, "Rail Detail\n\nSelected Rail: [to be chosen on Dashboard]\nVoltage: 5.00V\nCurrent: 0.00A\nCL Params: (if applicable)\nEnable/Disable: [TOGGLE]\nVolt Selector: (RailAdj only)");
    lv_obj_set_pos(placeholder, 40, 80);
    lv_obj_set_style_text_font(placeholder, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(placeholder, lv_color_hex(0xAAAAAA), 0);
}

static void create_screen_fault_lock() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    screens[static_cast<int>(ScreenId::FaultLock)] = screen;
    
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1A1A2E), 0);
    lv_obj_set_size(screen, 800, 480);
    
    lv_obj_t* alert = lv_label_create(screen);
    lv_label_set_text(alert, "⚠ FAULT LOCK ⚠");
    lv_obj_set_pos(alert, 200, 80);
    lv_obj_set_style_text_font(alert, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(alert, lv_color_hex(0xFF3333), 0);
    
    lv_obj_t* detail = lv_label_create(screen);
    lv_label_set_text(detail, "Fault Code: (fault code here)\nRail: (rail name)\nVoltage: (V at fault)\nCurrent: (A at fault)");
    lv_obj_set_pos(detail, 150, 200);
    lv_obj_set_style_text_font(detail, &lv_font_montserrat_16, 0);
    
    lv_obj_t* reset_btn = lv_btn_create(screen);
    lv_obj_set_size(reset_btn, 200, 80);
    lv_obj_set_pos(reset_btn, 300, 350);
    lv_obj_t* btn_label = lv_label_create(reset_btn);
    lv_label_set_text(btn_label, "RESET (hold 3s)");
    lv_obj_center(btn_label);
    lv_obj_set_style_bg_color(reset_btn, lv_color_hex(0xFF3333), 0);
}

static void create_screen_trend_view() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    screens[static_cast<int>(ScreenId::TrendView)] = screen;
    
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1A1A2E), 0);
    lv_obj_set_size(screen, 800, 480);
    
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Trend View (30s history)");
    lv_obj_set_pos(title, 20, 20);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    
#if LV_USE_CHART
    lv_obj_t* chart = lv_chart_create(screen);
    lv_obj_set_size(chart, 750, 400);
    lv_obj_set_pos(chart, 25, 60);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 60);  // 1 point/sec for 30s when sampled at 500ms
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 6, 2, true, 50);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 1, true, 50);
    
    // Placeholder: 4 voltage series (r0, r1, r2, in12) + 3 current series (r0, r1, r2)
    // Full implementation: use SquareLine to design chart layout
    
    lv_obj_t* placeholder = lv_label_create(screen);
    lv_label_set_text(placeholder, "Chart: 4V + 3I traces\n(LVGL chart 7-series)");
    lv_obj_set_pos(placeholder, 300, 200);
    lv_obj_set_style_text_font(placeholder, &lv_font_montserrat_14, 0);
#endif
}

static void create_screen_service_menu() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    screens[static_cast<int>(ScreenId::ServiceMenu)] = screen;
    
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1A1A2E), 0);
    lv_obj_set_size(screen, 800, 480);
    
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Service Menu");
    lv_obj_set_pos(title, 20, 20);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    
    lv_obj_t* menu = lv_label_create(screen);
    lv_label_set_text(menu, "• Current Limit Config\n• Operation Mode\n• Fault History\n• UART Diagnostics\n\nDiag: RX=0 TX=0 ERR=0");
    lv_obj_set_pos(menu, 40, 80);
    lv_obj_set_style_text_font(menu, &lv_font_montserrat_14, 0);
}

static void on_psu_status_update(const PsuStatus& status) {
    // Redraw active screen with new status data
    Serial.printf("[UI] Status update: state=%d mode=%s\n", static_cast<int>(status.state), status.mode);
    
    // Full implementation: update UI widget values via LVGL object tree
    // For now, just log to console
}

static void on_psu_fault(const char* faultCode, const RailStatus& rail) {
    // Switch to FaultLock screen
    Serial.printf("[UI] Fault event: %s on rail at %.2fV/%.3fA\n", faultCode, rail.voltageV, rail.currentA);
    
    if (activeScreen != ScreenId::FaultLock) {
        lv_scr_load(screens[static_cast<int>(ScreenId::FaultLock)]);
        activeScreen = ScreenId::FaultLock;
    }
}

static void on_psu_error(const char* error) {
    Serial.printf("[UI] PSU error: %s\n", error);
}

// ============================================================================
// Public API
// ============================================================================

void screen_manager_init() {
    // Create all 5 screens
    create_screen_dashboard();
    create_screen_rail_detail();
    create_screen_fault_lock();
    create_screen_trend_view();
    create_screen_service_menu();
    
    // Load Dashboard by default
    if (screens[static_cast<int>(ScreenId::Dashboard)] != nullptr) {
        lv_scr_load(screens[static_cast<int>(ScreenId::Dashboard)]);
        activeScreen = ScreenId::Dashboard;
        Serial.println("[Screen] Dashboard loaded");
    }
    
    // Register UART event callbacks
    uart_on_status_update(on_psu_status_update);
    uart_on_error(on_psu_error);
    uart_on_fault(on_psu_fault);
}

void screen_manager_load(ScreenId id) {
    if (id < ScreenId::Dashboard || id > ScreenId::ServiceMenu) return;
    if (screens[static_cast<int>(id)] == nullptr) return;
    
    lv_scr_load(screens[static_cast<int>(id)]);
    activeScreen = id;
}

ScreenId screen_manager_get_active() {
    return activeScreen;
}
