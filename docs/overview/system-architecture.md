# System Architecture

This document describes the high‑level structure of the DIY Hall Throttle system.

## Blocks

- Mechanics (pedal, spring, stops)
- Sensors (A1324 analog, SparkFun 23880 I²C)
- MCU (ATmega32U4, USB HID)
- PC UI (Windows, sidebar modes)

## Data Flow

```text
Mechanical motion 
    ↓
Sensor raw value
    ↓
Filtering
    ↓
Calibration (MIN / MAX)
    ↓
Deadzones
    ↓
Profile curve
    ↓
Output value
    ↓
USB HID report