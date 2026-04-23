# Phased Bring-Up Plan: PSU HAT ↔ CrowPanel UART Link

**Last Updated:** 2026-04-23  
**Canonical References:**  
- PSU Firmware: `Development-Station-Power-Supply/src/main.cpp` commit `900541a`
- HMI Firmware: `TTF-Display-board/firmware/` commit `03a500d`
- UART Protocol: `ELECROW_CROWPANEL_43_UART_PROTOCOL.md`

---

## Overview

This plan describes the step-by-step bench procedure to validate the PSU ↔ CrowPanel
UART link, from loopback test to live telemetry dashboard. Each phase builds on the
previous; completion of a phase proves that subsystem is ready.

---

## Phase A: UART Loopback & Serial Command Echo

**Objective:** Prove PSU firmware receives and parses CMD: frames, responds with ACK.

**Setup:**
1. Program PSU HAT board with `Development-Station-Power-Supply` commit `900541a` (includes `handleHmiCommand()`)
2. Connect PSU to bench power supply
3. Connect USB-to-UART TTL cable to PSU serial debug port (not UART0-IN — use the debug UART on the HAT)
4. Open terminal emulator (e.g., Tera Term, 115200 8N1) connected to debug UART
5. No CrowPanel yet; PSU runs standalone

**Test Sequence:**

```
[Terminal] > CMD:READ_STATUS
[PSU]      < ACK:READ_STATUS state=SAFE_IDLE mode=NORMAL r0=0.00/0.000 r1=0.00/0.000 r2=0.00/0.000 in=0.00/0.000 r2_sel=5V

[Terminal] > CMD:RAIL_ENABLE rail=Rail5V state=1
[PSU]      < ACK:RAIL_ENABLE rail=Rail5V state=1

[Terminal] > CMD:READ_STATUS
[PSU]      < ACK:READ_STATUS state=SAFE_IDLE mode=NORMAL r0=5.02/0.001 r1=0.00/0.000 r2=0.00/0.000 in=12.04/0.000 r2_sel=5V r0_en=1

[Terminal] > CMD:RAIL_ENABLE rail=Rail5V state=0
[PSU]      < ACK:RAIL_ENABLE rail=Rail5V state=0

[Terminal] > CMD:RESET
[PSU]      < ERR:RESET_DENIED Not in fault state
```

**Success Criteria:**
- ✅ PSU responds to every CMD: frame with ACK: or ERR:
- ✅ RAIL_ENABLE properly gates `hmiRailEnabled[]` state
- ✅ READ_STATUS returns correct voltage/current values from INA3221
- ✅ All responses are properly formatted (no garbage, framing correct)

**Failure Modes:**
- PSU ignores CMD: → check if serial parser branch was inserted correctly
- PSU responds with raw text (e.g. "MCP4231 wiper...") → old bare-token parser is being used
- Garbled response → baud rate mismatch or framing issue

---

## Phase B: CrowPanel Boot & Link Establish

**Objective:** Prove CrowPanel firmware boots, starts UART tasks, establishes link to PSU.

**Setup:**
1. Program CrowPanel with `TTF-Display-board/firmware` commit `03a500d`
2. Connect CrowPanel to PSU via UART0-IN connector (J_UART0 on CrowPanel, PSU UART TX/RX pins)
3. No display yet (will be black); CrowPanel powered via USB-CDC connector
4. PSU running from Phase A (loopback connection remains, for diagnostics)

**Test Sequence:**

1. Power on CrowPanel via USB
2. Open USB-CDC serial port (COM port on Windows) at 115200 8N1
3. Observe console output:
   ```
   ===== CrowPanel Advance 4.3 HMI Firmware =====
   ESP32-S3 @ 240 MHz
   PSRAM: 8.0 MB available
   [Init] LVGL...
   [Init] LVGL OK
   [Init] UART RX task...
   [Init] UART poll task...
   [Init] Screen manager...
   [Init] LVGL render task...
   [Boot] All systems online
   Waiting for PSU link establishment...
   ```
4. Observe periodic heartbeats:
   ```
   [HB] state=0 mode=NORMAL link=OK err=OK
       PSRAM: 1234 / 8192 KB used
   ```
5. After 500ms, check PSU loopback terminal: should see CMD:READ_STATUS sent
6. Within 1.5s, should see link=OK (not LOST)

**Success Criteria:**
- ✅ CrowPanel boots without fault
- ✅ UART RX/poll tasks start on cores 0 and 1
- ✅ First READ_STATUS is sent within 1000ms of boot
- ✅ PSU responds to first READ_STATUS within 200ms
- ✅ Link is marked OK (not LOST)
- ✅ Periodic CMD:READ_STATUS polling continues every 500ms
- ✅ Heartbeat shows PSRAM usage < 50%

**Failure Modes:**
- "PSRAM not detected" → check OPI mode flag in platformio.ini
- "Link=LOST" after boot → PSU not responding to CMD:READ_STATUS, check UART wiring
- Crash on boot → check lv_conf.h settings, LVGL draw buffer allocation

---

## Phase C: One Rail Enable & Telemetry Stream

**Objective:** Prove end-to-end control loop: CrowPanel sends CMD:RAIL_ENABLE, PSU applies, CrowPanel reads back voltage/current.

**Setup:**
1. Keep Phase B hardware running (CrowPanel ↔ PSU UART link live)
2. Add a 1A current sink to PSU Rail5V output (e.g., resistive load or dummy load module)
3. Optional: open PSU loopback terminal to watch CMD: exchange in real-time

**Test Sequence:**

1. From CrowPanel console (USB-CDC), inject a RAIL_ENABLE command via uart_send_command():
   ```
   > uart_send_command("CMD:RAIL_ENABLE rail=Rail5V state=1");
   ```
   Or add a test button to the Dashboard screen to trigger this via screen_manager.

2. Observe console:
   ```
   [HB] state=2 mode=NORMAL link=OK err=OK
       rail0: 5.02V / 1.00A (ENABLED)
       rail1: 3.30V / 0.00A (disabled)
       rail2: 0.00V / 0.00A (disabled)
       PSRAM: 1234 / 8192 KB used
   ```

3. Verify PSU loopback terminal shows:
   ```
   > CMD:RAIL_ENABLE rail=Rail5V state=1
   < ACK:RAIL_ENABLE rail=Rail5V state=1
   ```

4. Verify physical Rail5V output: scope shows 5V ±0.1V, current draws ~1.00A

5. From CrowPanel console, disable rail:
   ```
   > uart_send_command("CMD:RAIL_ENABLE rail=Rail5V state=0");
   ```

6. Observe state change back to disabled, voltage returns to 0V, current to 0A

**Success Criteria:**
- ✅ CMD:RAIL_ENABLE is sent without errors
- ✅ ACK:RAIL_ENABLE is received within 200ms
- ✅ Next READ_STATUS reflects enabled rail (state=ACTIVE)
- ✅ Voltage and current measurements are stable and correct
- ✅ No errors, no watchdog timeouts
- ✅ Disable works, state returns to SAFE_IDLE

**Failure Modes:**
- ACK not received → check UART wiring, baud rate
- Voltage doesn't rise on Rail5V → check rail enable GPIO pin assignment (currently -1 placeholder)
- Voltage rises but CrowPanel shows state=SAFE_IDLE → parsed response not reaching screen manager

---

## Phase D: Full Dashboard & Multi-Rail Test

**Objective:** Integrate display driver, render live Dashboard, control all 3 rails.

**Setup:**
1. Connect ST7265 LCD display to CrowPanel (via SquareLine-generated driver code)
2. Connect capacitive touch controller (firmware stubs present, full integration via SquareLine)
3. All prior connections remain (PSU, UART link, current sinks on all 3 rails)

**Test Sequence:**

1. Power on; CrowPanel should render Dashboard screen
2. Dashboard should show:
   - State badge (Safe Idle / Standby / Active Run)
   - Per-rail cards: Rail5V, Rail3V3, RailAdj with V/I and [ON|OFF] toggles
   - RailAdj selector: [5V] [3V3] exclusive buttons
   - Input 12V monitor display (read-only)
3. Tap Rail5V [ON] button → Rail5V powers on, voltage/current update every 100ms
4. Tap RailAdj [5V] selector (while disabled) → relay switches to 5V (no visual change yet)
5. Tap RailAdj [ON] button → RailAdj powers on at 5V
6. Tap RailAdj [3V3] selector (while ON) → should be grayed out (ERR:RAIL_ACTIVE expected)
7. Tap RailAdj [OFF] → disables rail, voltage drops
8. Now tap RailAdj [3V3] selector → relay switches to 3V3
9. Tap RailAdj [ON] → RailAdj powers on at 3V3
10. Tap "Rail Detail" navigation → Detail screen should show selected rail (last used)
11. Tap "Service Menu" → show diagnostics: RX/TX counts, error count, last PSU state

**Success Criteria:**
- ✅ All 3 rails respond to ON/OFF taps
- ✅ Voltage/current update smoothly, no glitches
- ✅ RailAdj selector works only when rail is off
- ✅ Touch is responsive, no lag
- ✅ Navigation between screens works
- ✅ All visual feedback matches state

**Failure Modes:**
- Display is black → display driver not integrated with ST7265
- Touch unresponsive → touch controller not wired or driver not initialized
- State changes lag by >500ms → screen update callback not firing
- UART link drops → check for task stack overflow, use `uart_get_pending()` to debug command stuck

---

## Phase E: Fault Injection & Recovery

**Objective:** Test fault detection (via current limit) and recovery path.

**Setup:**
1. Keep Phase D hardware running
2. Configure a CL setpoint on Rail5V: Iset=800mA, Icut=1.0A, mode=LATCH
   (Use PSU loopback: `CLSET Rail5V 800 1000 50; CLENABLE Rail5V ON`)
3. Load rail to just below trip: 0.95A
4. CrowPanel should remain in ACTIVE state

**Test Sequence:**

1. Slowly increase load above 1.0A (trigger OC_R0)
2. Observe: PSU detects overcurrent, enters FAULT state
3. PSU sends: `EVT:FAULT fault=OC_R0 rail=Rail5V v=5.01 i=1.05`
4. CrowPanel receives EVT:, navigates to **Fault Lock screen**
5. Fault Lock screen displays:
   - Red alert badge
   - Fault code: "OC_R0"
   - Rail: "Rail5V"
   - Snapshot: "5.01V / 1.05A"
6. User holds RESET button for 3 seconds
7. CrowPanel sends: `CMD:RESET`
8. PSU clears fault, responds: `ACK:RESET state=SAFE_IDLE`
9. CrowPanel navigates back to Dashboard
10. All rails are now off; state is SAFE_IDLE

**Success Criteria:**
- ✅ Fault is detected by PSU (current limit enforcement works)
- ✅ EVT:FAULT reaches CrowPanel within 100ms
- ✅ Screen switches to Fault Lock automatically
- ✅ Fault snapshot is displayed correctly
- ✅ RESET button works (3s hold)
- ✅ Recovery command is sent to PSU
- ✅ Dashboard reloads, state is correct

**Failure Modes:**
- PSU doesn't trip even though current > limit → CL mode not enabled, or setpoint not applied
- EVT:FAULT not received → check uart_on_fault() callback registration
- RESET button doesn't respond → touch driver issue, or button event not wired
- State not updated after RESET → pending command not cleared

---

## Phase F: Trend View & Service Menu

**Objective:** Validate chart rendering and diagnostics page.

**Setup:**
- Phase E hardware all working
- Optional: SquareLine studio has configured chart widget (currently placeholder)

**Test Sequence:**

1. Navigate to "Trend View" screen
2. Run normal operation for 30+ seconds (enable/disable rails, vary loads)
3. Chart should show 30s sliding window of 4 voltage traces (Rail5V, Rail3V3, RailAdj, 12V) + 3 current traces
4. Traces should update smoothly every poll cycle (every 100ms)
5. Navigate to "Service Menu"
6. Verify diagnostics show:
   - RX count (should be ≥ 6 per second, one per READ_STATUS poll)
   - TX count (should equal RX count)
   - ERR count (should be 0 unless errors were tested)
   - Last PSU mode, state, timestamp
7. Tap on "CL Config" section → show per-rail CL params (Iset, Icut, mode, etc.)

**Success Criteria:**
- ✅ Chart renders and updates smoothly
- ✅ Diagnostics page shows correct counts (sanity check: TX ≈ RX ≈ poll rate)
- ✅ No visual glitches, no crashes
- ✅ All screens accessible via navigation

**Failure Modes:**
- Chart doesn't update → chart callback not wired to status_update
- Diagnostics show TX=0, RX=0 → counters not being incremented
- Chart shows garbage values → measurement parsing wrong in parseReadStatusAck()

---

## Rollback & Debug Procedures

### If UART link is lost:
1. Check UART wiring (especially TX↔RX not swapped)
2. Verify baud rate is 115200 on both sides
3. Check PSU firmware is `900541a` or later (has handleHmiCommand())
4. Monitor PSU debug UART: does PSU see the frames it's sending?
5. If PSU sees frames but CrowPanel doesn't: check CrowPanel RX pin (IO44) logic level

### If Command doesn't apply (e.g., rail doesn't enable):
1. Check rail enable GPIO pins are assigned (currently all -1 in DSPS firmware)
2. Manually test GPIO via PSU bare-token commands: `P0 200` to toggle MCP4231
3. If GPIO is -1, command succeeds but hardware doesn't respond (expected until GPIO is wired)

### If state/mode doesn't update on screen:
1. Check screen_manager callbacks are registered: `uart_on_status_update()` called?
2. Enable debug logging: increase LV_LOG_LEVEL in lv_conf.h
3. Check pending command tracking: `uart_get_pending()` should be clear (type=None) after ACK

### If CrowPanel crashes:
1. Check PSRAM: `uart_get_psu_status()` is slow because it copies status struct
2. Check stack: 4KB RX task, 2KB poll task may be tight if callbacks are heavy
3. Use `Get-Symbol` on firmware.elf in GDB: validate memory layout

---

## Success Checklist

- [ ] Phase A: Loopback echo works, all CMD: frames parse
- [ ] Phase B: CrowPanel boots, link established, polling active
- [ ] Phase C: Single rail control and telemetry verified
- [ ] Phase D: Display renders, multi-rail control works, no lag
- [ ] Phase E: Fault injection, Fault Lock screen, recovery works
- [ ] Phase F: Trend view and Service Menu render correctly

Once all phases pass, the HMI is **ready for field deployment**.

---

## Next: PCB Hardware Hookup

With firmware validated, the next step is hardware integration:

1. **Rail enable GPIO assignment**: Assign `RAIL5V_ENABLE_PIN`, `RAIL3V3_ENABLE_PIN`, `RAILADJ_ENABLE_PIN` 
2. **RailAdj relay control**: Wire GPIO expander (I2C) for discrete relay switch (5V ↔ 3V3)
3. **Display driver integration**: Finalize SquareLine-generated LovyanGFX code for ST7265
4. **Touch driver**: Finalize capacitive touch via I2C (likely FT5426 or similar)
5. **Final PCB layout review**: Ensure UART0-IN connector is keyed to prevent swap

See [`REV_B_ACTION_LIST.md`](../../Captain-Fantastic-Control-and-Hat-Board/docs/REV_B_ACTION_LIST.md) for PCB revision schedule.
