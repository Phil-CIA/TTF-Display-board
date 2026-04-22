# Elecrow CrowPanel Advance 4.3 Connection Strategy

Last updated: 2026-04-22

## Objective
Use the CrowPanel as a self-hosted HMI node and connect it to the power-supply controller through a narrow, stable protocol interface.

## Recommended Architecture
1. CrowPanel onboard ESP32-S3 owns LCD, touch, and UI rendering.
2. Main controller board owns instrument control and safety logic.
3. Link the two with a simple framed command protocol over UART first.
4. Optionally add I2C command path later only if bench testing proves it necessary.

## Why This Path
- Avoids coupling external firmware to internal LCD/touch buses.
- Reduces migration risk from board revision differences.
- Keeps protocol and UI independent so hardware can be swapped later.

## Electrical Integration Baseline
- Common ground is mandatory before data lines.
- Power input should follow vendor guidance for 5V supply capacity.
- UART logic levels must stay at 3.3V logic domain.
- Do not drive unknown connector pins until revision is verified from board markings.

## Protocol Baseline (UART)
- Physical: 4-wire minimum (5V, GND, TX, RX).
- Suggested serial: 115200 8N1 for initial bring-up.
- Payload shape: line-oriented text commands with explicit ACK/ERR responses.

Example command envelope:
- `SET VOUT 12.00`
- `SET IOUT 1.000`
- `READ STATUS`
- `ACK <command-id>` or `ERR <code> <message>`

## Revision-Unknown Rule
When board revision is unknown:
1. Treat all connector pin assumptions as unverified.
2. Validate revision marker and silkscreen first.
3. Confirm pin map from vendor docs for that exact revision before wiring beyond USB.

## Debug and Flash Path
- Use onboard USB for flashing and serial logs first.
- Keep external host link disconnected until local UI boot and touch sanity checks pass.
- Only then introduce UART host link and command tests.

## Explicit Non-Goals
- No attempt to reuse internal display interface as a host bus.
- No dependency on AI/microphone/wireless expansion paths for initial power-supply UI integration.
