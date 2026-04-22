# Datasheets Folder

This folder is for locally mirrored datasheets used by the active integration path.

## Current State
- Mirrored files now include `esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf`.
- Use `../source-index/RESOURCE_INDEX.md` for provenance and checksums.

## Intended Contents
- ESP32-S3 module datasheet (exact module variant once confirmed on physical board).
- Touch controller datasheet used by the detected board revision.
- Any IO expander/backlight control component datasheets that impact bring-up.

## Mirroring Rule
Only mirror files that are directly relevant to active integration/debug tasks.

## Intake Checklist
1. Confirm physical board revision from silkscreen and photo evidence in `../images/`.
2. Confirm actual component identities from board and vendor source before mirroring files.
3. Download datasheet PDFs from official vendor/manufacturer pages.
4. Name mirrored files using `VENDOR_PART_REV.pdf` where possible.
5. Record source URL and retrieval date in `../source-index/RESOURCE_INDEX.md`.

## Revision-Gated Datasheet Matrix

| Component class | Expected source | Mirror now | Gate |
|---|---|---|---|
| MCU module (ESP32-S3-WROOM-1 family) | Espressif | Yes | Mirrored for current v1.1 work; N16R8 module class called out by vendor docs |
| Touch controller | Vendor board docs + controller vendor | Not yet | Controller identity must be confirmed per board revision |
| LCD driver/controller | Vendor board docs + controller vendor | Not yet | Controller identity must be confirmed per board revision |
| IO expander/backlight helper ICs | Vendor board docs + IC vendor | Not yet | Only mirror if used by active firmware/debug path |

## Mirrored Files
- `esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf` - official Espressif module datasheet PDF for the ESP32-S3-WROOM-1 / WROOM-1U family.
