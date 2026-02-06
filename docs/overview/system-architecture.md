# System Architecture

**Goal:** Describe high-level blocks, data flow, and modes.

## Blocks
- Mechanics (pedal, spring, stops)
- Sensors (A1324 analog, SparkFun 23880 I²C)
- MCU (ATmega32U4, USB HID)
- PC UI (Windows, sidebar: Driving | Settings | Fault | Diagnostics)

## Data Flow
```
Mechanical motion → Sensor raw → Filtering → Calibration (MIN/MAX) → Deadzones → Profile curve → Output → USB HID
                                              ↘ Diagnostics / Status via Serial ↗
```

## Modes
- **Driving**: live % and graph, profile indicator
- **Settings**: calibration, profile, deadzones, save to EEPROM
- **Fault**: error codes, guidance, safe output
- **Diagnostics**: raw/filtered/output values, noise, rates, counters
