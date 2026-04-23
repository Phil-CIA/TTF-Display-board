# PSU HMI Screen Map

**Capability source:** `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md`
**Operations rules:** `ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md`
**Last updated:** 2026-04-23
**Display target:** 800 x 480 IPS, 16-bit RGB565, capacitive touch

> All UI controls on this map are constrained to the channel capability profile.
> No analog setpoint sliders exist. RailAdj uses a 2-position selector only.

---

## Screen Navigation

```
  1. DASHBOARD (Safe Idle / Standby / Active Run)
     ├─ Tap rail row → 2. RAIL DETAIL (per-rail view)
     ├─ Tap FAULT alert → 3. FAULT LOCK
     ├─ Tap TRENDS → 4. TREND VIEW
     └─ Tap SERVICE → 5. SERVICE MENU

  3. FAULT LOCK
     └─ Hold RESET 3 s → 1. DASHBOARD (Safe Idle)
```

---

## Screen 1 — Dashboard

**States:** Safe Idle, Standby, Active Run (single screen, badge-driven)
**Resolution use:** Full 800 x 480

### Layout

```
┌──────────────────────────────────────────────────────────────┐
│  DEV-STATION PSU     Mode: [NORMAL]     12.05 V  0.90 A  12V │  ← top bar
├──────────────────────────────────────────────────────────────┤
│ Rail 5V   ● 5.02 V  1.20 A  [━━━━━━░░░░] 60%  [ON ●] [▶]   │  ← rail row
│ Rail 3V3  ● 3.31 V  0.80 A  [━━━━░░░░░░] 40%  [ON ●] [▶]   │
│ RailAdj  ○ 0.00 V  0.00 A  [░░░░░░░░░░]  0%  [OFF○] [5V|3V3] [▶] │
│                                                              │
│ (RailAdj selector [5V|3V3] active only when rail is OFF)    │
├──────────────────────────────────────────────────────────────┤
│   [START / STOP]       Mode:[NORMAL|CL]       [TRENDS] [SVC] │  ← footer
└──────────────────────────────────────────────────────────────┘
```

### Widgets

| Widget | Type | Capability constraint |
|---|---|---|
| **12V input bar (top right)** | Label | Read-only — `RailIncoming12V` monitor only |
| **Mode badge `[NORMAL]`** | Toggle button | Taps cycle NORMAL ↔ CURRENT_LIMIT; PSU ACK required |
| **Rail row — voltage / current** | Labels | Live 100 ms telemetry; read-only |
| **Current-bar `[━━░░]`** | Progress bar | % of current limit; visual only |
| **`[ON●] / [OFF○]`** | Toggle | 1-tap enable/disable; PSU ACK required |
| **`[5V\|3V3]` selector** | 2-position toggle | RailAdj only; grayed/hidden while rail is ON |
| **`[▶]`** | Icon button | Navigate to Screen 2 (rail detail) |
| **`[START]` / `[STOP]`** | Button | Transition Safe Idle→Standby or Active→Safe Idle |
| **`[TRENDS]`** | Button | Navigate to Screen 4 |
| **`[SVC]`** | Button | Navigate to Screen 5 |

### State-dependent badge behavior

| State | START/STOP label | Rail enable toggles | RailAdj selector |
|---|---|---|---|
| Safe Idle | START (green) | Disabled (gray) | Disabled |
| Standby | — | Enabled (tappable) | Enabled if rail off |
| Active Run | STOP (yellow) | Enabled (tappable) | Disabled while rail on |
| Fault Lock | RESET (red, 3 s hold) | Disabled | Disabled |

---

## Screen 2 — Rail Detail

**Entry:** Tap `[▶]` on any rail row from Dashboard
**Exit:** Tap `[BACK]`

### Layout

```
┌──────────────────────────────────────────────────────────────┐
│  ← BACK          RailAdj Detail                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Voltage:   5.01 V          Current:   0.45 A               │
│  CL Warn:  1000 mA          CL Trip:  1500 mA               │
│  CL Mode:  HICCUP           CL Delay:  200 ms / 800 ms      │
│                                                              │
│  Voltage select:  [ 5V ]  [ 3.3V ]   (active when OFF only) │
│                                                              │
│  Output:  [  ENABLE  ]                                       │
│                                                              │
│  Fault history:  No trips logged.                            │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### Widgets

| Widget | Type | Notes |
|---|---|---|
| **Voltage / Current labels** | Labels | Live 100 ms; read-only |
| **CL Warn / Trip / Mode / Delay** | Labels | Read from PSU status; read-only on this screen |
| **`[5V] [3.3V]`** | 2-button exclusive selector | RailAdj only; disabled when rail enabled |
| **`[ENABLE]` / `[DISABLE]`** | Button | Enable/disable this rail |
| **Fault history** | Label / scroll list | Last 5 trip events with timestamp |

> For Rail5V and Rail3V3, the voltage select row is not shown (fixed rails).

---

## Screen 3 — Fault Lock

**Entry:** Any LATCH-mode trip or UART watchdog timeout
**Exit:** 3 s hold on RESET → Safe Idle

### Layout

```
┌──────────────────────────────────────────────────────────────┐
│  ⚠ ⚠ ⚠  FAULT LOCK  ⚠ ⚠ ⚠                                  │  ← red bg
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Fault:  OC_R2  (RailAdj over-current trip)                  │
│  Mode:   LATCH                                               │
│                                                              │
│  Last reading:   5.01 V  /  1.62 A  at  14:32:08            │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   [RESET ▯▯▯  Hold 3 sec]                                    │  ← hold btn
│   All outputs held off until reset.                          │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### Widgets

| Widget | Type | Notes |
|---|---|---|
| **⚠ FAULT LOCK ⚠** | Label | Solid red background; white bold text |
| **Fault code + name** | Label | From fault code table in operations contract |
| **CL Mode** | Label | LATCH / HICCUP / MONITOR — set at fault entry |
| **Last reading snapshot** | Label | V, A, timestamp at fault moment |
| **`[RESET ▯▯▯]`** | Hold button | Progress arc fills over 3 s; sends `CMD:RESET` on complete |

---

## Screen 4 — Trend View (30 s)

**Entry:** Tap `[TRENDS]` from Dashboard
**Exit:** Tap `[BACK]`

### Layout

```
┌──────────────────────────────────────────────────────────────┐
│  TRENDS (30 s)                    ← BACK                     │
├──────────────────────────────────────────────────────────────┤
│ 5V:  ──────────────────────────── peak 5.04 / min 4.98      │
│ 3V3: ─────────────────────────────peak 3.32 / min 3.29      │
│ Adj: ─────────────────────────────peak 5.02 / min 4.99      │
│ 12V: ──────────────────────────── peak 12.1 / min 11.9      │
│ (line chart per rail, 0–30 s x-axis)                        │
├──────────────────────────────────────────────────────────────┤
│ Current:                                                     │
│ 5V:  ══════════════════════════   peak 1.25 / min 0.80      │
│ 3V3: ════════════════════════════ peak 0.82 / min 0.40      │
│ Adj: ════════════════════════════ peak 0.48 / min 0.10      │
└──────────────────────────────────────────────────────────────┘
```

### Widgets

| Widget | Type | Notes |
|---|---|---|
| **Voltage line chart (per rail)** | Line chart | 4 traces; 30 s ring buffer; 100 ms points |
| **Current line chart (per rail)** | Line chart | 3 traces (controllable rails); 30 s ring buffer |
| **Peak / Min labels** | Labels | Computed from ring buffer; yellow text |
| **`[BACK]`** | Button | Return to Dashboard |

> `RailIncoming12V` appears in voltage chart as a 4th trace (read-only monitor).
> It does not appear in the current chart with a separate trace — incoming supply
> current is shown in the 12V voltage row label only.

---

## Screen 5 — Service Menu

**Entry:** Tap `[SVC]` from Dashboard (Safe Idle or Standby only)
**Exit:** Tap `[EXIT]`

### Layout

```
┌──────────────────────────────────────────────────────────────┐
│  SERVICE MENU                               EXIT             │
├──────────────────────────────────────────────────────────────┤
│  ► Current-Limit Config (per rail)                           │
│  ► Operation Mode                                            │
│  ► Clear Fault History                                       │
│  ► View Audit Log                                            │
│  ► UART Diagnostics                                          │
│  ► About / Version                                           │
└──────────────────────────────────────────────────────────────┘

[Current-Limit Config expands:]
  Rail:  [Rail5V ▼]   Warn: 1500 mA   Trip: 2000 mA   Delay: 50 ms
  Mode:  [LATCH] [HICCUP●] [MONITOR]
  [SAVE ▯▯▯  Hold 5 sec]    [CANCEL]
```

### Widgets

| Widget | Type | Notes |
|---|---|---|
| **Rail selector `[Rail5V ▼]`** | Dropdown | Rail5V / Rail3V3 / RailAdj |
| **Warn / Trip / Delay** | Labels (read) + inputs in edit mode | Numeric values from NVS |
| **Mode selector `[LATCH][HICCUP][MONITOR]`** | 3-button exclusive | Current mode highlighted |
| **`[SAVE ▯▯▯]`** | Hold button | 5 s hold; sends CLSET + CFGSAVE serial commands |
| **`[CANCEL]`** | Button | Discard changes |
| **`[EXIT]`** | Button | Return to Dashboard |

> **No voltage setpoint inputs exist in this menu.** Rail voltages are hardware-
> fixed (Rail5V, Rail3V3) or relay-selected (RailAdj). Only current-limit
> parameters are configurable here.

---

## Color Palette

| Element | Color | Notes |
|---|---|---|
| Normal text | #333333 | Default labels |
| Active / enabled | #00AA00 | Green: ON toggle, START button |
| Warning / secondary | #FFAA00 | Yellow: STOP, info labels, peak markers |
| Fault / alert | #FF3333 | Red: FAULT LOCK header, fault badges |
| Disabled / grayed | #AAAAAA | Grayed controls when not applicable |
| Background | #1A1A2E | Dark navy: main background |
| Header bar | #16213E | Slightly lighter navy: top/footer bars |

---

## Interaction Model

| Gesture | Action |
|---|---|
| Single tap | Activate button; toggle state |
| Hold 3 s | RESET button (fault recovery) |
| Hold 5 s | SAVE button (service menu config) |
| Swipe up/down | Scroll within fault history or audit log |
| Tap outside modal | Dismiss non-critical overlays |

Touch debounce: 80 ms (CrowPanel local).

---

## SquareLine Export Notes

1. **No analog slider widget needed anywhere.** Remove any setpoint slider primitives from the template.
2. **RailAdj voltage selector:** implement as a 2-button exclusive group (one active at a time); bind to a boolean `railjAdj_is5V` state variable.
3. **Hold buttons (RESET / SAVE):** implement as transparent progress-arc overlay drawn each frame; send command on arc completion.
4. **Rail row current-bar:** standard LVGL bar widget, value = (current_mA / trip_mA) * 100.
5. **Font assets needed:** bold at 10 / 12 / 14 / 16 / 20 / 24 pt; regular at 10 / 12 / 14 pt.
6. **4 voltage traces + 3 current traces** in trend view — plan LVGL chart object with 7 series.
