# Elecrow CrowPanel Advance 4.3 Connection Strategy

Last updated: 2026-04-22

## Objective
Use the CrowPanel as a self-hosted HMI node and connect it to the power-supply controller through a narrow, stable protocol interface.

## Recommended Architecture
1. CrowPanel onboard ESP32-S3 owns LCD, touch, and UI rendering.
2. Main controller board owns instrument control and safety logic.
3. For the v1.1 board, link the two with a simple framed command protocol over UART0 first.
4. Optionally add I2C command path later only if bench testing proves it necessary.

## Why This Path
- Avoids coupling external firmware to internal LCD/touch buses.
- Reduces migration risk from board revision differences.
- Keeps protocol and UI independent so hardware can be swapped later.

## Electrical Integration Baseline
- Common ground is mandatory before data lines.
- Power input should follow vendor guidance for 5V supply capacity.
- UART logic levels must stay at 3.3V logic domain.
- This document assumes board revision v1.1.

## Recommended Electrical Interface To The Power-Supply Project
Use the CrowPanel `UART0-IN` connector as the primary installed interface to the power-supply project.

Recommended wiring:
- PSU 5V -> CrowPanel `UART0-IN` 5V input
- PSU GND -> CrowPanel GND
- PSU TX -> CrowPanel RX on IO44
- PSU RX <- CrowPanel TX on IO43

Why `UART0-IN` for v1.1:
- Vendor docs explicitly label it as the 5V power-input plus communication connector.
- It gives one installed interface for both operating power and host commands.
- `UART1` is less attractive because v1.1 docs also place microphone functions on IO19/IO20.
- `I2C` remains available for later expansion, but UART is simpler for first integration and logging.

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
Board revision is now treated as v1.1 for this integration path.

## Debug and Flash Path
- Use onboard USB for flashing and serial logs first.
- Keep external host link disconnected until local UI boot and touch sanity checks pass.
- During development, do not tie external PSU 5V and USB 5V together directly.
- For firmware loading and debug, prefer USB-only power.
- If simultaneous USB debug and host UART are needed, connect GND + TX + RX only and leave external 5V disconnected.
- Only move to full `UART0-IN` four-wire operation after standalone bring-up is stable.

## Integration Decision For The Power-Supply Project
Development/setup mode:
- USB-C only for flash, serial logs, and initial UI validation.

Installed/bench-integrated mode:
- One four-wire harness from power-supply controller to CrowPanel `UART0-IN` carrying 5V, GND, TX, RX.

Avoid for initial integration:
- Sharing USB 5V and project 5V without explicit power-path isolation.
- Using `UART1` until the v1.1 shared-function behavior on IO19/IO20 is proven harmless.
- Using I2C as the first host interface.

## Explicit Non-Goals
- No attempt to reuse internal display interface as a host bus.
- No dependency on AI/microphone/wireless expansion paths for initial power-supply UI integration.
