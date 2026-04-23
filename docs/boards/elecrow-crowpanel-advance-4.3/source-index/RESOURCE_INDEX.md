# Resource Index - Elecrow CrowPanel Advance 4.3

Last updated: 2026-04-22

## Policy
Hybrid artifact policy:
- Mirror small, stable, high-value text references in this repo.
- Keep large external assets as indexed links with provenance.

## Official Sources

| Source | URL | Type | Local mirror | Notes |
|---|---|---|---|---|
| Elecrow wiki page | https://www.elecrow.com/wiki/CrowPanel_Advance_4.3-HMI_ESP32_AI_Display.html | Product docs | No | Primary source for specs, interfaces, revision notes |
| Elecrow board repo | https://github.com/Elecrow-RD/CrowPanel-Advance-4.3--HMI-ESP32-800x480 | Vendor code/docs | No | Primary source for examples and board-specific firmware patterns |
| Espressif module docs | https://www.espressif.com/en/products/modules/esp32-s3-wroom-1 | MCU module reference | No | Module family used by board line |
| Espressif module datasheet PDF | https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf | Datasheet PDF | Yes | Mirrored locally as `../datasheets/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf` |
| CrowPanel code library (Drive) | https://drive.google.com/drive/folders/1npWkVzctd7e0Y6H5hahk8uxGhy9idxCz?usp=sharing | External bundle | No | Vendor-hosted assets; mirror selectively if required |
| CrowPanel wireless bundle (Drive) | https://drive.google.com/drive/folders/1DWLBHqny0IwR9hPPtEwjRyMHmVubhqIj?usp=sharing | External bundle | No | Optional expansion module references |
| PlatformIO tutorial v1.1 | https://www.elecrow.com/wiki/CrowPanel_advance_5.0_ESP32_AI_display_with_PlatformIO_V1.1.html | Tutorial | No | Official PlatformIO bring-up guide for v1.1 |
| SquareLine Studio tutorial v1.1 | https://www.elecrow.com/wiki/3Use_SquareLine_Studio_and_LVGL_libraries_to_create_a_UI_interface_to_light_the_lights.html | Tutorial | No | Official SquareLine + LVGL UI design workflow for v1.1 |

## Link Validation Snapshot (2026-04-22)

| URL | Status | Notes |
|---|---|---|
| https://www.elecrow.com/wiki/CrowPanel_Advance_4.3-HMI_ESP32_AI_Display.html | 200 | Primary board documentation page reachable |
| https://github.com/Elecrow-RD/CrowPanel-Advance-4.3--HMI-ESP32-800x480 | 200 (redirect) | Redirects to `CrowPanel-Advance-4.3-HMI-ESP32-S3-AI-Powered-IPS-Touch-Screen-800x480` |
| https://www.espressif.com/en/products/modules/esp32-s3-wroom-1 | 200 | ESP32-S3 module family page reachable |
| https://drive.google.com/drive/folders/1npWkVzctd7e0Y6H5hahk8uxGhy9idxCz?usp=sharing | 200 | Vendor code library folder reachable |
| https://drive.google.com/drive/folders/1DWLBHqny0IwR9hPPtEwjRyMHmVubhqIj?usp=sharing | 200 | Vendor wireless folder reachable |

## Technical Facts Pulled For Bring-Up
- Display resolution: 800×480.
- Panel type: IPS (not TN) — confirmed from vendor spec.
- LCD driver IC: **ST7265** — confirmed from vendor spec page.
- Main module class: ESP32-S3-WROOM-1-N16R8.
- External power target: 5V/2A (wiki indicates 5V±5%, 5.5V max on UART0-IN, 2A max).
- Touch: **capacitive** (resistive stylus included separately for precision), controlled via **STC8H1K28** I/O expander.
- Touch I2C address: 0x5D (confirmed for v1.0; treat as working assumption for v1.1 until bench confirmed).
- Board revision for current work: v1.1.
- v1.1 external host candidates from vendor docs: UART0 on IO44/IO43, UART1 on IO19/IO20, I2C on IO15/IO16.
- **v1.1 caution:** IO19 and IO20 are shared between UART1 and I2S MIC. Do not use UART1 as the host interface unless bench testing confirms the shared behavior is acceptable.
- Backlight, buzzer, TP_RST, and power amplifier SHUT are all controlled by the **STC8H1K28** I/O expander, not directly by ESP32-S3 GPIOs.
- SD card on v1.1 shares IO4/IO5/IO6 with the I2S speaker output. Switching keys (S0/S1) select active function.
- SquareLine Studio is the recommended UI design workflow for this board.

## Mirroring Backlog
- Mirror STC8H1K28 datasheet — controls backlight, buzzer, TP_RST, and power amplifier SHUT on v1.1; needed once any of those functions enter the active firmware path.
- Mirror ST7265 LCD driver datasheet — needed if custom init sequence work is required beyond vendor examples.
- Add board photos in `../images/` that identify revision markers and connector orientation.
- Add checksums for any mirrored binaries once mirrored files are introduced.

## Next Artifact Intake Targets
1. Mirror touch controller datasheet only after controller identity is confirmed from board revision and firmware config.
2. Capture a board-revision evidence set in `../images/` before pin-map-specific notes are treated as authoritative.
3. Mirror any control-helper IC datasheets only if they enter the active firmware/debug path.

## Mirrored Artifact Record

| File | Source URL | Retrieved | SHA256 |
|---|---|---|---|
| `../datasheets/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf` | https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf | 2026-04-22 | `27D71971DA07C280C6068D08C74720D1A25B8F20CF8494DC1765BDD28D40D435` |
