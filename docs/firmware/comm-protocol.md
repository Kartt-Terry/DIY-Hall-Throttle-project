# UI ↔ Firmware Communication Protocol

This document defines the minimal communication protocol between the configuration UI and the firmware running on the Arduino Pro Micro.

## Commands (examples)

Below are example commands sent **from UI to firmware**.

```json
{
  "cmd": "set_mode",
  "mode": "diagnostics"
  "cmd": "calibrate_min"
  "cmd": "calibrate_max"
  "cmd": "save"
  "cmd": "set_profile",
  "profile": "exponential",
  "expo": 1.8
  "cmd": "set_deadzones",
  "low": 0.02,
  "high": 0.03
  "status": "OK",
  "mode": "driving",
  "raw": 523,
  "filtered": 517,
  "norm": 0.48,
  "out": 0.45,
  "profile": "linear"
  "status": "FAULT",
  "error": "SENSOR_NOT_DETECTED",
  "code": 2

}
