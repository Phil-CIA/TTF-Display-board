/**
 * include/uart_link.h — PSU UART interface API
 *
 * Public task starters and command senders for UART0-IN <-> PSU link.
 * All tasks are started from main(), all comms handled asynchronously.
 */
#pragma once

#include "hmi_state.h"
#include <stdint.h>

/* Start the UART RX task on core 0 (4KB stack)
 * Reads lines from UART0, dispatches ACK/ERR/EVT to callbacks
 */
extern void uart_rx_task_start();

/* Start the periodic status poll task on core 1 (2KB stack)
 * Sends CMD:READ_STATUS every 500ms when idle
 * Tracks 1500ms watchdog for link loss detection
 */
extern void uart_poll_task_start();

/* Send a complete command line (without \r\n, added internally) */
extern void uart_send_command(const char* cmd);

/* Queue a pending command for status tracking */
extern void uart_set_pending(PendingCommand::Type type, const char* payload);

/* Get current PSU status from last successful READ_STATUS ACK */
extern const PsuStatus& uart_get_psu_status();

/* Get current pending command state */
extern PendingCommand uart_get_pending();

/* Callback signatures for async event delivery to screen manager */
typedef void (*uart_event_callback_t)(const PsuStatus& status);
typedef void (*uart_error_callback_t)(const char* error);
typedef void (*uart_fault_callback_t)(const char* faultCode, const RailStatus& rail);

/* Register event callbacks (called from uart_rx_task) */
extern void uart_on_status_update(uart_event_callback_t cb);
extern void uart_on_error(uart_error_callback_t cb);
extern void uart_on_fault(uart_fault_callback_t cb);

/* Diagnostics */
extern uint32_t uart_get_rx_count();
extern uint32_t uart_get_tx_count();
extern uint32_t uart_get_err_count();
