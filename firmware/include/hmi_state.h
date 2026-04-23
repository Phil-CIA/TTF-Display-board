/**
 * include/hmi_state.h — Shared HMI state types
 *
 * Defines the state machine state enum, rail measurement struct,
 * and pending command tracking for UART <-> screen synchronization.
 */
#pragma once

#include <stdint.h>
#include <cmath>

enum class HmiState : uint8_t {
    SafeIdle    = 0,  // All rails off, no comms fault
    Standby     = 1,  // At least one rail on, Normal or CL mode selected
    ActiveRun   = 2,  // Rails actively outputting current
    FaultLock   = 3,  // OC/thermal trip detected, user holds RESET
};

enum class ScreenId : uint8_t {
    Dashboard    = 0,   // Rail status, per-rail toggle, RailAdj volt selector
    RailDetail   = 1,   // Selected rail: CL params, volt selector, enable/disable
    FaultLock    = 2,   // Red alert, fault code snapshot, RESET button
    TrendView    = 3,   // 30s history: 4V traces + 3I traces (LVGL chart)
    ServiceMenu  = 4,   // CL config, mode, fault history, UART diagnostics
};

struct RailStatus {
    float voltageV;
    float currentA;
    bool enabled;
    bool inFault;
    uint8_t faultCode;  // 0 = none, 1 = OC_R0, 2 = OC_R1, 3 = OC_R2
};

struct RailAdjStatus : RailStatus {
    bool is5V;  // true = 5V selected, false = 3V3 selected
};

struct PsuStatus {
    HmiState state;
    char mode[16];              // "NORMAL" or "CURRENT_LIMIT"
    RailStatus rail0;           // Rail5V
    RailStatus rail1;           // Rail3V3
    RailAdjStatus rail2;        // RailAdj
    RailStatus incomingRail;    // 12V in (read-only)
    uint32_t lastUpdateMs;
    bool linkLost;              // No ACK from PSU in 1500ms
    char lastError[80];         // Last ERR: or status message
};

struct PendingCommand {
    enum Type : uint8_t {
        None,
        ReadStatus,
        RailEnable,
        RailVoltSel,
        SetMode,
        Reset,
        Abort,
    } type;
    
    uint32_t sentMs;
    uint32_t timeoutMs;
    char payload[128];  // For logging/debugging
};

inline bool isFinite(float v) {
    return std::isfinite(v) && !std::isnan(v);
}

inline float clampVoltage(float v) {
    return isFinite(v) ? v : 0.0f;
}

inline float clampCurrent(float i) {
    return isFinite(i) ? i : 0.0f;
}
