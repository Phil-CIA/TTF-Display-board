# PSU HMI UART Protocol Specification

**Operations contract:** `ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md`
**Capability source:** `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md`
**Last updated:** 2026-04-23

---

## Purpose

Formal field-level encoding for the UART link between the Elecrow CrowPanel
(initiator) and the PSU HAT board firmware (responder). This document is the
implementer reference for both sides.

---

## Physical Layer

| Parameter | Value |
|---|---|
| Baud | 115200 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Line ending | CR+LF (`\r\n`) — both sides must accept LF-only for robustness |
| Logic level | 3.3 V |
| Max frame length | 128 bytes (including line ending) |
| Framing | Newline-delimited text; one command or response per line |

---

## Frame Format

### Command (CrowPanel → PSU)

```
CMD:<ACTION> [<key>=<value>[ <key>=<value>...]]\r\n
```

Rules:
- `CMD:` prefix required, uppercase
- `<ACTION>` uppercase, no spaces
- `key=value` pairs separated by single space
- Keys and values are case-insensitive on the PSU side (PSU uppercases on receipt)
- No trailing space before `\r\n`
- Max total frame length: 128 bytes

### Response (PSU → CrowPanel)

Success:
```
ACK:<ACTION> [<key>=<value>[ <key>=<value>...]]\r\n
```

Error:
```
ERR:<ERROR_CODE> <human-readable message>\r\n
```

Unsolicited event (PSU-initiated, e.g. fault):
```
EVT:<EVENT_CODE> [<key>=<value>[ <key>=<value>...]]\r\n
```

---

## Rail ID Tokens

Used wherever `rail=<id>` appears. Both forms accepted on receive; canonical form
used in responses.

| Rail | Canonical token | Aliases accepted |
|---|---|---|
| Rail5V | `Rail5V` | `5V`, `RAIL5V`, `R0` |
| Rail3V3 | `Rail3V3` | `3V3`, `3.3`, `RAIL3V3`, `R1` |
| RailAdj | `RailAdj` | `ADJ`, `RAILADJ`, `R2` |
| RailIncoming12V | `RailIn12V` | `IN12`, `INCOMING`, `12V` — read-only, no control commands |

---

## Commands

### CMD:READ_STATUS

Poll current PSU state and all rail telemetry.

**Request:**
```
CMD:READ_STATUS\r\n
```

**Response fields:**

```
ACK:READ_STATUS state=<STATE> mode=<MODE> r0=<V>/<I> r1=<V>/<I> r2=<V>/<I> in=<V>/<I>\r\n
```

| Field | Type | Example | Notes |
|---|---|---|---|
| `state` | string | `SAFE_IDLE` | One of: `SAFE_IDLE`, `STANDBY`, `ACTIVE`, `FAULT` |
| `mode` | string | `NORMAL` | One of: `NORMAL`, `CURRENT_LIMIT` |
| `r0` | `<V_float>/<I_float>` | `5.02/1.20` | Rail5V voltage (V) / current (A); 2 decimal places |
| `r1` | `<V_float>/<I_float>` | `3.31/0.80` | Rail3V3 voltage / current |
| `r2` | `<V_float>/<I_float>` | `5.01/0.45` | RailAdj voltage / current |
| `in` | `<V_float>/<I_float>` | `12.05/0.90` | Incoming 12V rail (monitor only) |

**Extended fields (optional, included when non-nominal):**

| Field | Type | Example | Notes |
|---|---|---|---|
| `r0_en` | `0` or `1` | `r0_en=1` | Rail enable state; omit when 1 (nominal) |
| `r1_en` | `0` or `1` | `r1_en=0` | |
| `r2_en` | `0` or `1` | `r2_en=0` | |
| `r2_sel` | `5V` or `3V3` | `r2_sel=5V` | RailAdj voltage selection; always present |
| `fault` | string | `fault=OC_R2` | Fault code; only present in FAULT state |

**Example response — Active Run, all nominal:**
```
ACK:READ_STATUS state=ACTIVE mode=NORMAL r0=5.02/1.20 r1=3.31/0.80 r2=5.01/0.45 in=12.05/0.90 r2_sel=5V\r\n
```

**Example response — Fault state:**
```
ACK:READ_STATUS state=FAULT mode=NORMAL r0=5.00/0.00 r1=3.30/0.00 r2=0.00/0.00 in=12.02/0.40 r2_sel=5V fault=OC_R2\r\n
```

---

### CMD:RAIL_ENABLE

Enable or disable a single controllable rail output.

**Request:**
```
CMD:RAIL_ENABLE rail=<id> state=<0|1>\r\n
```

| Field | Values | Notes |
|---|---|---|
| `rail` | Rail ID token | `RailIncoming12V` is rejected (monitor only) |
| `state` | `0` or `1` | `1` = enable, `0` = disable |

**Response:**
```
ACK:RAIL_ENABLE rail=<canonical-id> state=<0|1>\r\n
```

**Error cases:**
```
ERR:RAIL_UNKNOWN Unknown rail id\r\n
ERR:RAIL_READONLY Rail is monitor-only, cannot be controlled\r\n
ERR:RAIL_FAULT Rail is in fault state; send CMD:RESET first\r\n
```

**Example:**
```
> CMD:RAIL_ENABLE rail=RailAdj state=1\r\n
< ACK:RAIL_ENABLE rail=RailAdj state=1\r\n
```

---

### CMD:RAIL_VOLT_SEL

Select the discrete output voltage for RailAdj (relay select).
**Only valid when RailAdj is disabled (state=0).**

**Request:**
```
CMD:RAIL_VOLT_SEL rail=RailAdj volt=<5V|3V3>\r\n
```

| Field | Values | Notes |
|---|---|---|
| `rail` | `RailAdj` only | Any other rail returns `ERR:RAIL_FIXED` |
| `volt` | `5V` or `3V3` | No other values accepted |

**Response:**
```
ACK:RAIL_VOLT_SEL rail=RailAdj volt=<5V|3V3>\r\n
```

**Error cases:**
```
ERR:RAIL_FIXED Rail voltage is hardware-fixed, cannot be selected\r\n
ERR:RAIL_ACTIVE Rail must be disabled before changing voltage selection\r\n
ERR:VOLT_INVALID Voltage value not valid for this rail\r\n
```

**Example — Changing RailAdj to 3.3 V while disabled:**
```
> CMD:RAIL_VOLT_SEL rail=RailAdj volt=3V3\r\n
< ACK:RAIL_VOLT_SEL rail=RailAdj volt=3V3\r\n
```

**Example — Rejected because rail is currently enabled:**
```
> CMD:RAIL_VOLT_SEL rail=RailAdj volt=3V3\r\n
< ERR:RAIL_ACTIVE Rail must be disabled before changing voltage selection\r\n
```

---

### CMD:SET_MODE

Switch PSU system operation mode.

**Request:**
```
CMD:SET_MODE mode=<NORMAL|CURRENT_LIMIT>\r\n
```

**Response:**
```
ACK:SET_MODE mode=<NORMAL|CURRENT_LIMIT>\r\n
```

**Error:**
```
ERR:MODE_INVALID Unknown mode token\r\n
```

**Example:**
```
> CMD:SET_MODE mode=CURRENT_LIMIT\r\n
< ACK:SET_MODE mode=CURRENT_LIMIT\r\n
```

---

### CMD:RESET

Clear a fault condition and return to Safe Idle. PSU only accepts this when in FAULT state.

**Request:**
```
CMD:RESET\r\n
```

**Response — success:**
```
ACK:RESET state=SAFE_IDLE\r\n
```

**Response — rejected (not in fault state):**
```
ERR:RESET_DENIED Not in fault state\r\n
```

---

### CMD:ABORT

Hard stop: disable all controllable rail outputs immediately, regardless of current state.

**Request:**
```
CMD:ABORT\r\n
```

**Response:**
```
ACK:ABORT state=SAFE_IDLE\r\n
```

This command is always accepted. It is the escape hatch for communication
reliability failures.

---

## Unsolicited Events (PSU → CrowPanel)

The PSU may send these at any time, without a preceding command.
CrowPanel must handle them gracefully even when not expecting them.

### EVT:FAULT

Sent when a current-limit LATCH trip occurs.

```
EVT:FAULT fault=<CODE> rail=<id> v=<V> i=<I>\r\n
```

| Field | Example | Notes |
|---|---|---|
| `fault` | `OC_R2` | Fault code (see fault code table) |
| `rail` | `RailAdj` | Rail that tripped |
| `v` | `5.01` | Voltage at fault time (V) |
| `i` | `1.62` | Current at fault time (A) |

**Example:**
```
EVT:FAULT fault=OC_R2 rail=RailAdj v=5.01 i=1.62\r\n
```

### EVT:WATCHDOG

Sent by PSU before it triggers soft shutdown due to UART idle timeout (1 s).
This gives CrowPanel a last-chance notice.

```
EVT:WATCHDOG idle_ms=<ms>\r\n
```

---

## Error Codes

| Code | Meaning |
|---|---|
| `UNKNOWN_CMD` | Unrecognized `CMD:` action token |
| `PARSE_ERROR` | Malformed frame (missing required key, bad value format) |
| `RAIL_UNKNOWN` | Rail ID token not recognized |
| `RAIL_READONLY` | Attempted control on monitor-only rail |
| `RAIL_FIXED` | Attempted voltage selection on a fixed-voltage rail |
| `RAIL_ACTIVE` | Attempted relay voltage select while rail is enabled |
| `RAIL_FAULT` | Rail is in LATCH fault; must RESET before enabling |
| `VOLT_INVALID` | Voltage token not valid for this rail |
| `MODE_INVALID` | Operation mode token not recognized |
| `RESET_DENIED` | RESET sent outside FAULT state |

---

## Sequence Diagrams

### Boot Handshake

```
CrowPanel boots                      PSU boots (may be earlier)
        │                                     │
        │── (wait 500 ms for PSU ready) ──────│
        │                                     │
        │──── CMD:READ_STATUS ───────────────>│
        │                                     │ (parse, gather telemetry)
        │<─── ACK:READ_STATUS state=SAFE_IDLE ─│
        │                                     │
    [Update UI with received state]          │
        │                                     │
        │  (repeat every 500 ms until ACTIVE) │
```

If no ACK within 200 ms, retry up to 3×. If all retries fail, show
"No Connection" banner and disable all controls.

---

### Normal Operation — Rail Enable

```
Operator taps [ON] for Rail5V
        │
        │──── CMD:RAIL_ENABLE rail=Rail5V state=1 ───────>│
        │                                                  │ (apply enable)
        │<─── ACK:RAIL_ENABLE rail=Rail5V state=1 ─────────│
        │
    [Update Rail5V badge → ON, green]
        │
        │  (100 ms poll cycle continues)
        │──── CMD:READ_STATUS ─────────────────────────────>│
        │<─── ACK:READ_STATUS state=ACTIVE r0=5.02/1.20 ────│
```

---

### RailAdj Voltage Change (Safe Sequence)

```
Operator taps [3V3] selector while RailAdj is OFF
        │
        │──── CMD:RAIL_VOLT_SEL rail=RailAdj volt=3V3 ────>│
        │<─── ACK:RAIL_VOLT_SEL rail=RailAdj volt=3V3 ──────│
        │
    [Update selector badge → 3V3]
        │
    Operator taps [ON] for RailAdj
        │──── CMD:RAIL_ENABLE rail=RailAdj state=1 ─────── >│
        │<─── ACK:RAIL_ENABLE rail=RailAdj state=1 ─────────│
        │
    [Update RailAdj badge → ON, green]
```

---

### Fault Event and Recovery

```
        │                                PSU detects OC_R2
        │                                         │
        │<─── EVT:FAULT fault=OC_R2 rail=RailAdj v=5.01 i=1.62 ─│
        │
    [Show FAULT LOCK screen]
    [Snapshot: RailAdj, 5.01V, 1.62A]
        │
    Operator holds RESET for 3 s
        │──── CMD:RESET ──────────────────────────────────── >│
        │<─── ACK:RESET state=SAFE_IDLE ─────────────────────│
        │
    [Navigate back to Dashboard, state=SAFE_IDLE]
```

---

## PSU Firmware Integration Notes

The existing `handleSerialCommands()` in `main.cpp` parses bare uppercase tokens
(`CLSET 5V 1500 2000 50`). The `CMD:` framing from CrowPanel is a **new parallel
parser** sitting alongside the existing human/debug parser.

### Implementation approach

Add a second branch at the top of `handleSerialCommands()`:

```cpp
if (command.startsWith("CMD:")) {
  handleHmiCommand(command);
  return;
}
// ... existing bare-token parser continues unchanged
```

`handleHmiCommand()` parses `CMD:<ACTION> key=value ...` and dispatches to the
same underlying functions already used by the bare-token parser
(`applyOperationMode()`, `setRailOutputEnabled()`, etc.).

This preserves all existing bench/debug serial commands unchanged while adding
the new structured HMI interface.

### Key firmware stubs needed (not yet implemented)

| Stub | Maps to |
|---|---|
| `CMD:RAIL_ENABLE` → `setRailOutputEnabled(rail, true/false)` | `RAIL5V_ENABLE_PIN` etc. — currently -1 placeholders; assign before enabling |
| `CMD:RAIL_VOLT_SEL` → relay GPIO write | GPIO expander not yet in firmware |
| `CMD:SET_MODE` → `applyOperationMode()` | Already works via `tftRequestedOperationMode` |
| `CMD:READ_STATUS` → read INA3221 + format response | `readRailMeasurement()` already exists |
| `CMD:RESET` → clear fault state | `clearCurrentLimitTrip()` already exists |
| `EVT:FAULT` push → hook into CL trip path | Serial.printf in `runCurrentLimitCycle()` → replace with `emitFaultEvent()` |

---

## CrowPanel Firmware Integration Notes

### UART task skeleton

```
Task: uart_rx_task (core 0, 4KB stack)
  - Read Serial1 (UART0-IN connector) line by line
  - On \n: dispatch to processIncoming(line)
    - if starts with "ACK:" → match to pending command, release semaphore
    - if starts with "ERR:" → log error, update UI error state
    - if starts with "EVT:" → push event to UI queue

Task: uart_poll_task (core 1, 2KB stack)
  - Every 500 ms idle: send CMD:READ_STATUS
  - Watchdog: if no ACK in 1500 ms → set link_lost flag → UI shows No Connection
```

### Pending command tracking

CrowPanel should queue only one command at a time (simplex operation).
Send → wait for ACK/ERR → send next. Do not pipeline commands.
