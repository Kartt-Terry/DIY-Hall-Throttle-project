# UI ↔ Firmware Communication Protocol

**Transport:** Serial (115200), JSON lines (`
` delimited).

## Commands (examples)
```json
{ "cmd": "set_mode", "mode": "diagnostics" }
{ "cmd": "calibrate_min" }
{ "cmd": "calibrate_max" }
{ "cmd": "save" }
{ "cmd": "set_profile", "profile": "exponential", "expo": 1.8 }
{ "cmd": "set_deadzones", "low": 0.02, "high": 0.03 }
```

## Telemetry (examples)
```json
{ "status": "OK", "mode": "driving", "raw": 523, "filtered": 517, "norm": 0.48, "out": 0.45, "profile": "linear" }
{ "status": "FAULT", "error": "SENSOR_NOT_DETECTED", "code": 2 }
```
