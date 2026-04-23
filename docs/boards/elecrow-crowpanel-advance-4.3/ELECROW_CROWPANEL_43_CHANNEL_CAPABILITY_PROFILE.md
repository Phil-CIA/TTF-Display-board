# PSU Channel Capability Profile

**Canonical source:** Development-Station-Power-Supply repo (`src/main.cpp`, `HANDOFF.md`)
**Last confirmed:** 2026-04-23
**Applies to:** All Elecrow CrowPanel HMI UI docs for this power-supply project

---

## Purpose

This document is the single authoritative capability map for the UI.  
Every HMI control, screen widget, and operations contract rule must trace back to this profile.  
If hardware capability changes, update this file first, then cascade to dependent docs.

---

## Rail Summary

| Rail ID | Firmware Name | Fixed Voltage | Control Type | UI Control |
|---|---|---|---|---|
| 0 | `Rail5V` | 5.0 V | Enable / Disable only | On/Off toggle |
| 1 | `Rail3V3` | 3.3 V | Enable / Disable only | On/Off toggle |
| 2 | `RailAdj` | 5.0 V **or** 3.3 V | Discrete voltage select + Enable / Disable | 2-position selector + On/Off toggle |
| — | `RailIncoming12V` | 12 V input | Monitor only (no control) | Read-only display |

---

## Rail 0 — Rail5V (Fixed 5 V)

- **Output voltage:** Fixed 5.0 V (hardware-determined, not software-settable)
- **Output current:** Controlled by current-limit firmware (LATCH / HICCUP / MONITOR modes)
- **Current limit range:** Configurable via serial (`CLSET 0 ...`); stored in NVS
- **Safe bring-up limit:** 1500 mA warn / 2000 mA trip / 50 ms delay
- **UI operations allowed:**
  - View live voltage and current (read-only telemetry)
  - Enable or disable rail output (on/off toggle only)
  - View current-limit status and trip history (read-only)
- **UI operations NOT allowed:**
  - Setting output voltage (fixed rail; no setpoint UI)
  - Continuous setpoint adjustment slider

---

## Rail 1 — Rail3V3 (Fixed 3.3 V)

- **Output voltage:** Fixed 3.3 V (hardware-determined, not software-settable)
- **Output current:** Controlled by current-limit firmware
- **Current limit range:** Configurable via serial (`CLSET 1 ...`); stored in NVS
- **Safe bring-up limit:** 1500 mA warn / 2000 mA trip / 50 ms delay
- **UI operations allowed:**
  - View live voltage and current (read-only telemetry)
  - Enable or disable rail output (on/off toggle only)
  - View current-limit status and trip history (read-only)
- **UI operations NOT allowed:**
  - Setting output voltage (fixed rail; no setpoint UI)
  - Continuous setpoint adjustment slider

---

## Rail 2 — RailAdj (Discrete 5 V / 3.3 V Select)

- **Output voltage:** Discrete select only — either 5.0 V or 3.3 V
  - Selected by relay / GPIO expander on the HAT board
  - Selection is a hardware relay state change, **not** a DAC or PWM setpoint
  - No intermediate voltages are available or valid
- **Output current:** Controlled by current-limit firmware
- **Current limit range:** Configurable via serial (`CLSET 2 ...`); stored in NVS
- **Safe bring-up limit:** 1000 mA warn / 1500 mA trip / 50 ms delay
  (lower than Rail5V / Rail3V3 — note tighter limits)
- **UI operations allowed:**
  - View live voltage and current (read-only telemetry)
  - Enable or disable rail output (on/off toggle only)
  - Select voltage level: 5 V or 3.3 V (2-position toggle or selector widget only)
  - View current-limit status and trip history (read-only)
- **UI operations NOT allowed:**
  - Continuous setpoint adjustment slider (relay is discrete; no analog setpoint exists)
  - Any value other than 5 V or 3.3 V

---

## Incoming 12 V — Monitor Only

- **Input:** External 12 V supply input to the regulator
- **Measured by:** INA3221 channel 3 with `ch3VoltageScale` / `ch3CurrentScale` applied
- **Firmware status:** Read-only; not a controllable rail
- **UI operations allowed:**
  - View live input voltage and current draw (read-only telemetry)
- **UI operations NOT allowed:**
  - Any enable/disable or setpoint control

---

## Current-Limit Operating Modes (All Controllable Rails)

| Mode | Behavior |
|---|---|
| `LATCH` | Trip and latch output off; operator must clear fault via RESET |
| `HICCUP` | Trip, hold off for delay window, then attempt auto-recovery |
| `MONITOR` | Log and alert only; do not cut output |

Default bring-up mode: `HICCUP` on all three rails.

---

## Operation Mode (System-Level)

The PSU firmware supports two system-level modes selectable from the TFT/HMI:

| Mode | Behavior |
|---|---|
| `NORMAL` | Standard operation; current-limit enforcement active |
| `CURRENT_LIMIT` | Enhanced current-limit mode; tighter enforcement policy |

Mode is toggled by writing `tftRequestedOperationMode` from the display firmware.

---

## Guardrail — Capability Drift Warning

If any of the following change on the hardware side, this profile **must** be updated before
any UI doc or screen design is modified:

- Relay hardware added or changed for RailAdj voltage selection
- Current-limit pin assignments finalized (currently -1 placeholder in firmware)
- Additional rails or enable GPIOs assigned
- INA3221 channel mapping changed

Cross-reference firmware constants to validate:
- `RAIL5V_ENABLE_PIN`, `RAIL3V3_ENABLE_PIN`, `RAILADJ_ENABLE_PIN`
- `ch3VoltageScale`, `ch3CurrentScale`
- `CLSET` serial command defaults in `HANDOFF.md`
