# UI ↔ Firmware Communication Protocol

This document defines the minimal communication protocol between the configuration UI and the firmware.

## Commands (examples)

Below are example commands sent from UI to firmware.

```json
{
  "cmd": "set_mode",
  "mode": "diagnostics"
}