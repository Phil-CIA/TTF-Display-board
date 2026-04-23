# Elecrow CrowPanel Advance 4.3 Board Pack

Board documentation bundle for the Elecrow CrowPanel Advance 4.3 inch HMI (Amazon listing B0G3WCDFLD), maintained as a bench-first integration reference.

Confirmed working assumption for this pack: board revision v1.1.

## Purpose
- Centralize official references for the board in one place.
- Provide a safe bring-up path when board revision is unknown.
- Define the recommended integration path into the main power-supply project.

## Start Here
1. Read `source-index/RESOURCE_INDEX.md` for official source links and provenance.
2. Read `ELECROW_CROWPANEL_43_CONNECTION_STRATEGY.md` for wiring and integration approach.
3. Run `ELECROW_CROWPANEL_43_FIRST_POWERUP.md` on first bench power-up.
4. Use USB for firmware loading and serial debug during setup; use the v1.1 external host interface only after standalone bring-up is stable.

## UI Design Start Here
Read these in order before designing any screens or writing any HMI firmware:

1. `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md` â€” authoritative map of what the PSU hardware can actually do. **Read this first.** All UI rules derive from it.
2. `ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md` â€” state machine, control authority, UART protocol, and safety rules.
3. `ELECROW_CROWPANEL_43_SCREEN_MAP.md` â€” 5-screen layout with widget types, state-dependent behavior, and SquareLine export notes.
- `ELECROW_CROWPANEL_43_UART_PROTOCOL.md` — UART frame format, command reference, error codes, sequence diagrams

> Channel capability source: Development-Station-Power-Supply repo (`src/main.cpp`, `HANDOFF.md`), confirmed 2026-04-23.
> If PSU hardware capabilities change, update `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md` first.

## Folder Layout
- `source-index/` - URL registry and what is mirrored vs external.
- `datasheets/` - Local mirrors and pointers to vendor datasheets.
- `images/` - Board photos and revision-identification captures.
- `ELECROW_CROWPANEL_43_CHANNEL_CAPABILITY_PROFILE.md` - PSU rail capability map (canonical source for all UI docs).
- `ELECROW_CROWPANEL_43_OPERATIONS_CONTRACT.md` - HMI state machine, control authority, UART protocol.
- `ELECROW_CROWPANEL_43_SCREEN_MAP.md` - 5-screen layout and SquareLine design notes.
- `ELECROW_CROWPANEL_43_UART_PROTOCOL.md` — UART frame format, command reference, error codes, sequence diagrams

## Scope Decision
As of 2026-04-22, this board is treated as an off-the-shelf HMI candidate. This repo stores integration guidance and reference artifacts only, not production firmware for the board.

