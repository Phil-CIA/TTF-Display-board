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
| CrowPanel code library (Drive) | https://drive.google.com/drive/folders/1npWkVzctd7e0Y6H5hahk8uxGhy9idxCz?usp=sharing | External bundle | No | Vendor-hosted assets; mirror selectively if required |
| CrowPanel wireless bundle (Drive) | https://drive.google.com/drive/folders/1DWLBHqny0IwR9hPPtEwjRyMHmVubhqIj?usp=sharing | External bundle | No | Optional expansion module references |

## Technical Facts Pulled For Bring-Up
- Display resolution: 800x480.
- Main module class: ESP32-S3-WROOM-1 variant.
- External power target: 5V (wiki indicates 5V/2A class external supply).
- Touch path appears I2C on vendor materials; common address notes include 0x5D (some refs also mention 0x14).
- Board has documented version deltas (v1.0/v1.1/v1.2 family notes in vendor material); do not assume one fixed pin map until revision is identified.

## Mirroring Backlog
- Add local PDF mirrors for core silicon/component datasheets used by active firmware path.
- Add board photos in `../images/` that identify revision markers and connector orientation.
- Add checksums for any mirrored binaries once mirrored files are introduced.
