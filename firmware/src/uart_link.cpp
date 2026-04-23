/**
 * src/uart_link.cpp — PSU UART link implementation
 *
 * Two FreeRTOS tasks:
 *  - uart_rx_task (core 0): reads lines, parses ACK:/ERR:/EVT:, dispatches callbacks
 *  - uart_poll_task (core 1): sends CMD:READ_STATUS every 500ms, watches for link loss
 *
 * Simplex command model: one command pending at a time, wait for ACK/ERR before next.
 */
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "hmi_state.h"
#include "uart_link.h"
#include <cstring>
#include <cstdio>

// UART port assignment from platformio.ini flags
#ifndef PSU_UART_TX_PIN
#  define PSU_UART_TX_PIN 43
#endif
#ifndef PSU_UART_RX_PIN
#  define PSU_UART_RX_PIN 44
#endif
#ifndef PSU_UART_BAUD
#  define PSU_UART_BAUD 115200
#endif

// Global state
static PsuStatus gPsuStatus = {
    .state = HmiState::SafeIdle,
    .mode = "NORMAL",
    .lastUpdateMs = 0,
    .linkLost = true,
};

static PendingCommand gPending = {
    .type = PendingCommand::None,
    .sentMs = 0,
    .timeoutMs = 200,
};

static SemaphoreHandle_t gStatusMutex = nullptr;
static SemaphoreHandle_t gPendingMutex = nullptr;

static uint32_t gRxCount = 0;
static uint32_t gTxCount = 0;
static uint32_t gErrCount = 0;

static uart_event_callback_t gStatusCallback = nullptr;
static uart_error_callback_t gErrorCallback = nullptr;
static uart_fault_callback_t gFaultCallback = nullptr;

// Forward declarations
static void parseReadStatusAck(const String& ack);
static void parseEventFault(const String& evt);

/**
 * uart_rx_task — Read lines from UART0-IN, dispatch to handlers
 *
 * Expected frames:
 *  ACK:READ_STATUS state=ACTIVE mode=NORMAL r0=5.02/1.20 ...
 *  ERR:ERROR_CODE message text
 *  EVT:FAULT fault=OC_R2 rail=RailAdj v=5.01 i=1.62
 *  EVT:WATCHDOG idle_ms=1234
 */
static void uart_rx_task(void* arg) {
    HardwareSerial& uart = Serial1;
    uart.begin(PSU_UART_BAUD, SERIAL_8N1, PSU_UART_RX_PIN, PSU_UART_TX_PIN);
    
    Serial.println("[UART] RX task started");
    
    while (true) {
        if (uart.available()) {
            String line = uart.readStringUntil('\n');
            line.trim();
            if (line.isEmpty()) continue;
            
            gRxCount++;
            
            if (line.startsWith("ACK:")) {
                parseReadStatusAck(line);
            } else if (line.startsWith("ERR:")) {
                gErrCount++;
                const int spacePos = line.indexOf(' ', 4);
                if (spacePos > 0) {
                    String msg = line.substring(spacePos + 1);
                    {
                        xSemaphoreTake(gStatusMutex, portMAX_DELAY);
                        snprintf(gPsuStatus.lastError, sizeof(gPsuStatus.lastError), "ERR: %s", msg.c_str());
                        gPsuStatus.linkLost = false;
                        xSemaphoreGive(gStatusMutex);
                    }
                    if (gErrorCallback) {
                        gErrorCallback(msg.c_str());
                    }
                }
            } else if (line.startsWith("EVT:")) {
                if (line.startsWith("EVT:FAULT")) {
                    parseEventFault(line);
                }
            }
        }
        taskYIELD();
    }
}

/**
 * uart_poll_task — Send CMD:READ_STATUS every 500ms, detect link loss
 *
 * Watchdog: if no ACK for 1500ms, set linkLost flag and disable UI controls.
 * Retry logic: up to 3× on timeout before giving up.
 */
static void uart_poll_task(void* arg) {
    HardwareSerial& uart = Serial1;
    uint32_t lastStatusMs = 0;
    uint32_t retryCount = 0;
    const uint32_t POLL_INTERVAL_MS = 500;
    const uint32_t WATCHDOG_TIMEOUT_MS = 1500;
    const uint32_t CMD_TIMEOUT_MS = 200;
    const uint32_t MAX_RETRIES = 3;
    
    Serial.println("[UART] Poll task started");
    delay(500);  // Wait for RX task to initialize
    
    while (true) {
        uint32_t now = millis();
        
        // Periodic status poll
        if (now - lastStatusMs >= POLL_INTERVAL_MS) {
            {
                xSemaphoreTake(gPendingMutex, portMAX_DELAY);
                if (gPending.type == PendingCommand::None) {
                    // Send CMD:READ_STATUS
                    uart.print("CMD:READ_STATUS\r\n");
                    gTxCount++;
                    gPending.type = PendingCommand::ReadStatus;
                    gPending.sentMs = now;
                    strncpy(gPending.payload, "CMD:READ_STATUS", sizeof(gPending.payload) - 1);
                    retryCount = 0;
                }
                xSemaphoreGive(gPendingMutex);
            }
            lastStatusMs = now;
        }
        
        // Watchdog: check for ACK timeout
        {
            xSemaphoreTake(gPendingMutex, portMAX_DELAY);
            if (gPending.type != PendingCommand::None) {
                uint32_t elapsed = now - gPending.sentMs;
                if (elapsed > CMD_TIMEOUT_MS) {
                    retryCount++;
                    if (retryCount < MAX_RETRIES) {
                        // Retry
                        uart.print(gPending.payload);
                        uart.print("\r\n");
                        gTxCount++;
                        gPending.sentMs = now;
                    } else {
                        // Give up
                        {
                            xSemaphoreTake(gStatusMutex, portMAX_DELAY);
                            gPsuStatus.linkLost = true;
                            snprintf(gPsuStatus.lastError, sizeof(gPsuStatus.lastError), 
                                   "Link timeout after %d retries", (int)MAX_RETRIES);
                            xSemaphoreGive(gStatusMutex);
                        }
                        gPending.type = PendingCommand::None;
                        retryCount = 0;
                    }
                }
                if (elapsed > WATCHDOG_TIMEOUT_MS && gPending.type != PendingCommand::None) {
                    Serial.printf("[UART] Watchdog: no ACK for %lu ms\n", elapsed);
                }
            }
            xSemaphoreGive(gPendingMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void parseReadStatusAck(const String& ack) {
    // ACK:READ_STATUS state=ACTIVE mode=NORMAL r0=5.02/1.20 r1=3.31/0.80 r2=5.01/0.45 in=12.05/0.90 r2_sel=5V
    
    {
        xSemaphoreTake(gStatusMutex, portMAX_DELAY);
        
        // Parse state
        int pos = ack.indexOf("state=");
        if (pos > 0) {
            int ePos = ack.indexOf(' ', pos);
            if (ePos < 0) ePos = ack.length();
            String state = ack.substring(pos + 6, ePos);
            if (state == "SAFE_IDLE")      gPsuStatus.state = HmiState::SafeIdle;
            else if (state == "STANDBY")   gPsuStatus.state = HmiState::Standby;
            else if (state == "ACTIVE")    gPsuStatus.state = HmiState::ActiveRun;
            else if (state == "FAULT")     gPsuStatus.state = HmiState::FaultLock;
        }
        
        // Parse mode
        pos = ack.indexOf("mode=");
        if (pos > 0) {
            int ePos = ack.indexOf(' ', pos);
            if (ePos < 0) ePos = ack.length();
            ack.substring(pos + 5, ePos).toCharArray(gPsuStatus.mode, sizeof(gPsuStatus.mode) - 1);
        }
        
        // Parse per-rail telemetry: r0=5.02/1.20 means r0.voltageV=5.02, r0.currentA=1.20
        auto parseRail = [&](const String& key, RailStatus& rail) {
            int p = ack.indexOf(key + "=");
            if (p > 0) {
                int ePos = ack.indexOf(' ', p);
                if (ePos < 0) ePos = ack.length();
                String vals = ack.substring(p + key.length() + 1, ePos);
                int slashPos = vals.indexOf('/');
                if (slashPos > 0) {
                    rail.voltageV = vals.substring(0, slashPos).toFloat();
                    rail.currentA = vals.substring(slashPos + 1).toFloat();
                }
            }
        };
        
        parseRail("r0", gPsuStatus.rail0);
        parseRail("r1", gPsuStatus.rail1);
        parseRail("r2", gPsuStatus.rail2);
        parseRail("in", gPsuStatus.incomingRail);
        
        // Parse r2_sel (RailAdj voltage selection)
        pos = ack.indexOf("r2_sel=");
        if (pos > 0) {
            int ePos = ack.indexOf(' ', pos);
            if (ePos < 0) ePos = ack.length();
            gPsuStatus.rail2.is5V = (ack.substring(pos + 7, ePos) == "5V");
        }
        
        // Parse fault code if present
        pos = ack.indexOf("fault=");
        if (pos > 0) {
            int ePos = ack.indexOf(' ', pos);
            if (ePos < 0) ePos = ack.length();
            String fault = ack.substring(pos + 6, ePos);
            if (fault == "OC_R0") {
                gPsuStatus.rail0.inFault = true;
                gPsuStatus.rail0.faultCode = 1;
            } else if (fault == "OC_R1") {
                gPsuStatus.rail1.inFault = true;
                gPsuStatus.rail1.faultCode = 2;
            } else if (fault == "OC_R2") {
                gPsuStatus.rail2.inFault = true;
                gPsuStatus.rail2.faultCode = 3;
            }
        }
        
        // Parse enable states (r0_en=0, r1_en=0, etc.) — when not present, assume 1 (nominal)
        auto parseEnable = [&](const String& key, RailStatus& rail) {
            int p = ack.indexOf(key + "=");
            if (p > 0) {
                int ePos = ack.indexOf(' ', p);
                if (ePos < 0) ePos = ack.length();
                rail.enabled = (ack.substring(p + key.length() + 1, ePos) == "1");
            } else {
                rail.enabled = true;  // Default to nominal
            }
        };
        
        parseEnable("r0_en", gPsuStatus.rail0);
        parseEnable("r1_en", gPsuStatus.rail1);
        parseEnable("r2_en", gPsuStatus.rail2);
        
        gPsuStatus.lastUpdateMs = millis();
        gPsuStatus.linkLost = false;
        strncpy(gPsuStatus.lastError, "OK", sizeof(gPsuStatus.lastError) - 1);
        
        xSemaphoreGive(gStatusMutex);
    }
    
    // Clear pending command
    {
        xSemaphoreTake(gPendingMutex, portMAX_DELAY);
        gPending.type = PendingCommand::None;
        xSemaphoreGive(gPendingMutex);
    }
    
    // Invoke callback
    if (gStatusCallback) {
        xSemaphoreTake(gStatusMutex, portMAX_DELAY);
        PsuStatus copy = gPsuStatus;
        xSemaphoreGive(gStatusMutex);
        gStatusCallback(copy);
    }
}

static void parseEventFault(const String& evt) {
    // EVT:FAULT fault=OC_R2 rail=RailAdj v=5.01 i=1.62
    RailStatus rail = {};
    String faultCode;
    
    int pos = evt.indexOf("fault=");
    if (pos > 0) {
        int ePos = evt.indexOf(' ', pos);
        if (ePos < 0) ePos = evt.length();
        faultCode = evt.substring(pos + 6, ePos);
    }
    
    pos = evt.indexOf(" v=");
    if (pos > 0) {
        int ePos = evt.indexOf(' ', pos + 1);
        if (ePos < 0) ePos = evt.length();
        rail.voltageV = evt.substring(pos + 3, ePos).toFloat();
    }
    
    pos = evt.indexOf(" i=");
    if (pos > 0) {
        int ePos = evt.indexOf(' ', pos + 1);
        if (ePos < 0) ePos = evt.length();
        rail.currentA = evt.substring(pos + 3, ePos).toFloat();
    }
    
    Serial.printf("[UART] FAULT event: %s at %.2fV/%.3fA\n", faultCode.c_str(), rail.voltageV, rail.currentA);
    
    {
        xSemaphoreTake(gStatusMutex, portMAX_DELAY);
        gPsuStatus.state = HmiState::FaultLock;
        xSemaphoreGive(gStatusMutex);
    }
    
    if (gFaultCallback) {
        gFaultCallback(faultCode.c_str(), rail);
    }
}

// ============================================================================
// Public API implementations
// ============================================================================

void uart_rx_task_start() {
    if (gStatusMutex == nullptr) {
        gStatusMutex = xSemaphoreCreateMutex();
        gPendingMutex = xSemaphoreCreateMutex();
    }
    xTaskCreatePinnedToCore(uart_rx_task, "uart_rx", 4096, nullptr, 2, nullptr, 0);
}

void uart_poll_task_start() {
    xTaskCreatePinnedToCore(uart_poll_task, "uart_poll", 2048, nullptr, 1, nullptr, 1);
}

void uart_send_command(const char* cmd) {
    HardwareSerial& uart = Serial1;
    uart.print(cmd);
    uart.print("\r\n");
    gTxCount++;
}

void uart_set_pending(PendingCommand::Type type, const char* payload) {
    xSemaphoreTake(gPendingMutex, portMAX_DELAY);
    gPending.type = type;
    gPending.sentMs = millis();
    strncpy(gPending.payload, payload, sizeof(gPending.payload) - 1);
    xSemaphoreGive(gPendingMutex);
}

const PsuStatus& uart_get_psu_status() {
    static PsuStatus copy;
    xSemaphoreTake(gStatusMutex, portMAX_DELAY);
    copy = gPsuStatus;
    xSemaphoreGive(gStatusMutex);
    return copy;
}

PendingCommand uart_get_pending() {
    xSemaphoreTake(gPendingMutex, portMAX_DELAY);
    PendingCommand copy = gPending;
    xSemaphoreGive(gPendingMutex);
    return copy;
}

void uart_on_status_update(uart_event_callback_t cb) {
    gStatusCallback = cb;
}

void uart_on_error(uart_error_callback_t cb) {
    gErrorCallback = cb;
}

void uart_on_fault(uart_fault_callback_t cb) {
    gFaultCallback = cb;
}

uint32_t uart_get_rx_count() { return gRxCount; }
uint32_t uart_get_tx_count() { return gTxCount; }
uint32_t uart_get_err_count() { return gErrCount; }
