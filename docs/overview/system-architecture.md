# System Architecture

This file describes the high level design.

## Blocks

- Mechanics (pedal, spring, stops)
- Sensors (A1324 analog, SparkFun 23880 I²C)
- MCU (ATmega32U4, USB HID)
- PC UI (Windows)

## Data Flow

```text
Mechanical → Raw sensor → Filtering → Calibration → Deadzones → Profile
→ Output → USB HID
```

## Modes

- Driving
- Settings
- Fault
- Diagnostics
