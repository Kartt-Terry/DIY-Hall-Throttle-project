# UI ↔ Firmware Communication Protocol

This file defines a small, stable protocol between the Windows UI and the
firmware.

## Commands (examples)

Below are example commands sent from UI to firmware.

```json
{
  "cmd": "set_mode",
  "mode": "diagnostics"
}
```

```json
{
  "cmd": "calibrate_min"
}
```

```json
{
  "cmd": "calibrate_max"
}
```

```json
{
  "cmd": "save"
}
```

```json
{
  "cmd": "set_profile",
  "profile": "exponential",
  "expo": 1.8
}
```

```json
{
  "cmd": "set_deadzones",
  "low": 0.02,
  "high": 0.03
}
```

## Telemetry (examples)

Messages sent from firmware to UI.

```json
{
  "status": "OK",
  "mode": "driving",
  "raw": 523,
  "filtered": 517,
  "norm": 0.48,
  "out": 0.45,
  "profile": "linear"
}
```

```json
{
  "status": "FAULT",
  "error": "SENSOR_NOT_DETECTED",
  "code": 2
}
```
