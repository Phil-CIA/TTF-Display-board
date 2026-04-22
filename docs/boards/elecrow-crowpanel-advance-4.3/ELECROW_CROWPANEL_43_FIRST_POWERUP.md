# Elecrow CrowPanel Advance 4.3 First Power-Up Checklist

Last updated: 2026-04-22

## Goal
Reach stable boot, display output, touch response, and serial logging with no external host connection.

## Required Before Power
- Bench supply set for vendor-recommended 5V operation.
- USB cable known-good for data.
- No external wiring except USB on first test.
- Camera/phone ready to capture board silkscreen and revision markers.

## Phase 1 - Visual and Revision Identification
1. Photograph front and back of the board.
2. Record any revision text and PCB identifiers.
3. Record connector labels exactly as printed.
4. Save photos under `images/` for future traceability.

## Phase 2 - Power and Boot
1. Apply USB power only.
2. Confirm power LED state.
3. Observe screen initialization behavior.
4. Open serial monitor and capture boot log.

Pass criteria:
- Device enumerates over USB serial.
- Boot output appears and is repeatable.
- Display exits blank state.

## Phase 3 - Touch and UI Sanity
1. Run vendor sample that matches board revision.
2. Confirm at least one touch event is detected.
3. Confirm touch coordinates are stable and plausible.

## Phase 4 - External Interface Bring-Up
1. Connect GND between host and CrowPanel.
2. Connect UART TX/RX at 3.3V logic levels.
3. Run one command/response loopback scenario.
4. Add power sharing only after communications are stable.

## Failure Signatures and First Checks
- No serial output:
  - Check cable data capability and COM selection.
  - Re-test with reset/boot sequence.
- White or black screen:
  - Verify firmware example matches board revision.
  - Confirm panel init path from vendor sample.
- Touch not responding:
  - Verify configured I2C pins/address for detected revision.
  - Confirm touch-related control lines are initialized in sample code.
- Random resets:
  - Check supply stability and inrush behavior.
  - Remove external wiring and retest USB-only state.

## Exit Criteria
- Board revision documented.
- Boot log captured.
- Display render confirmed.
- Touch confirmed.
- UART command link confirmed.
