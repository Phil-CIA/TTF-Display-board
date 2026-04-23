# PSU HMI Operations Contract

**Canonical capability source:** `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md`
**Last updated:** 2026-04-23

> **Important:** All operational rules in this document derive from the channel
> capability profile. If capability changes, update the profile first, then
> return here to revise any affected rules.

---

## Purpose

Defines the HMI behavioral rules, state machine, safety policies, and UART
protocol baseline for the Elecrow CrowPanel display operating as a front panel
for the Development-Station-Power-Supply project.

---

## System Roles

| Role | Responsibility |
|---|---|
| **CrowPanel ESP32-S3** | UI rendering, touch handling, state display. Requests actions from PSU over UART. Never autonomously enables outputs. |
| **PSU Firmware (HAT board)** | Owns output switching, current-limit enforcement, fault detection, and NVS config. ACKs or rejects HMI requests. |

---

## Operational States

### State 1 — Safe Idle
- Entry: power-on default, or after fault recovery.
- All rail outputs held off.
- Operator taps START to move to Standby.
- Display: "SAFE IDLE" with rail status badges (all off).

### State 2 — Standby
- Entry: operator confirms START; PSU acknowledges.
- Rails remain off until operator selects which rails to enable.
- Idle timeout: 180 s with no input → revert to Safe Idle.
- Display: "STANDBY" with per-rail enable toggles visible.

### State 3 — Active Run
- Entry: operator enables at least one rail from Standby.
- Live telemetry updating; current-limit enforcement active.
- Operator taps STOP to disable all rails and return to Safe Idle.
- Display: "ACTIVE RUN" with live V / I per rail.

### State 4 — Fault Lock
- Entry: any current-limit trip in LATCH mode, or UART watchdog timeout.
- All controllable rails held off by PSU.
- Operator must hold RESET for 3 s to clear and return to Safe Idle.
- Display: RED ALERT banner, fault rail ID, fault mode, last telemetry snapshot.

---

## Control Authority

| Action | Confirmation Required |
|---|---|
| View telemetry | None |
| Enable / disable a rail | 1-tap toggle; PSU ACK required |
| Select RailAdj voltage (5 V or 3.3 V) | 1-tap on selector; only valid when rail is OFF; PSU ACK required |
| Switch system operation mode (NORMAL / CURRENT_LIMIT) | 1-tap on mode badge; PSU ACK required |
| STOP (disable all rails) | 1-tap STOP button |
| Fault Lock recovery | 3 s continuous hold on RESET button |
| Adjust current-limit config | Service menu only; 5 s hold to save |
| Clear fault history | Service menu; confirm dialog |

> **RailAdj voltage selection rule:** The 5 V / 3.3 V selector for Rail 2 must
> only be active when the rail output is disabled. The UI must disable or hide
> the selector while the rail is on to prevent a live relay switching event.

---

## UART Protocol

### Baseline
- Baud: 115200 8N1, newline-delimited text (CR+LF)
- CrowPanel TX → PSU RX; PSU TX → CrowPanel RX

### Command Format
```
CMD:ACTION key=value [key=value ...]
```
Response:
```
ACK:<cmd-id> [key=value ...]
ERR:<error-code> <message>
```

### Core Commands

| Command | Example | PSU Response |
|---|---|---|
| `CMD:RAIL_ENABLE rail=<id> state=<0\|1>` | `CMD:RAIL_ENABLE rail=Rail5V state=1` | `ACK:RAIL_ENABLE rail=Rail5V state=1` |
| `CMD:RAIL_VOLT_SEL rail=RailAdj volt=<5V\|3V3>` | `CMD:RAIL_VOLT_SEL rail=RailAdj volt=3V3` | `ACK:RAIL_VOLT_SEL rail=RailAdj volt=3V3` |
| `CMD:SET_MODE mode=<NORMAL\|CURRENT_LIMIT>` | `CMD:SET_MODE mode=CURRENT_LIMIT` | `ACK:SET_MODE mode=CURRENT_LIMIT` |
| `CMD:READ_STATUS` | `CMD:READ_STATUS` | `ACK:READ_STATUS state=ACTIVE r0=5.02/1.20 r1=3.31/0.80 r2=5.01/0.45 in=12.05/0.90` |
| `CMD:RESET` | `CMD:RESET` | `ACK:RESET state=SAFE_IDLE` |
| `CMD:ABORT` | `CMD:ABORT` | `ACK:ABORT state=SAFE_IDLE` |

### Timeout and Retry Rules

| Scenario | Timeout | Action |
|---|---|---|
| ACK not received | 200 ms | Retry up to 3×; fault if all fail |
| Incomplete frame | 50 ms | Discard; wait for next newline |
| No command sent (idle) | 500 ms | CrowPanel sends `CMD:READ_STATUS` |
| PSU heartbeat missing | 1500 ms | CrowPanel shows "No Connection"; disables controls |
| PSU watchdog (no valid UART) | 1000 ms | PSU triggers soft shutdown of all rails |

---

## Safety Invariants

1. **No autonomous enable:** CrowPanel never enables a rail without an explicit operator tap.
2. **RailAdj relay change only when off:** `CMD:RAIL_VOLT_SEL` is only issued when that rail's enable state is 0. PSU must reject the command if rail is on.
3. **Comms loss locks controls:** If heartbeat missing >1500 ms, all enable/control UI elements are disabled until link restores.
4. **Fault latching:** In LATCH mode a tripped rail stays off until explicit `CMD:RESET` from operator.
5. **Ramp policy:** PSU handles all output ramping internally. HMI does not control ramp timing.

---

## Refresh Rates

| Element | Rate | Notes |
|---|---|---|
| V / I telemetry | 100 ms | 10 Hz poll from `CMD:READ_STATUS` |
| Rail status badges | Immediate on event, 250 ms cosmetic | State-change driven |
| Fault / alert banners | Immediate | Event-driven |
| Touch debounce | 80 ms | CrowPanel local |
| Idle session timeout | 180 s | Revert Standby → Safe Idle |

---

## Fault Codes

| Code | Meaning | Recovery |
|---|---|---|
| `OC_R0` | Rail5V over-current trip | Check load, RESET |
| `OC_R1` | Rail3V3 over-current trip | Check load, RESET |
| `OC_R2` | RailAdj over-current trip | Check load, RESET |
| `COMMS` | UART link lost >1500 ms | Check cable; RESET when link restores |
| `WATCHDOG` | PSU received no UART for 1 s | Check CrowPanel; RESET |

---

## Decisions Made

- RailAdj voltage selection requires rail to be OFF first (prevents live relay switching).
- No analog setpoint sliders anywhere in the HMI (no continuously variable outputs exist).
- System mode toggle (NORMAL / CURRENT_LIMIT) is always accessible from the main run screen.
- Fault recovery uses 3 s hold to prevent accidental clear.
- All three controllable rails share the same current-limit mode UI; no per-rail mode picker on the main screen (service menu only).
