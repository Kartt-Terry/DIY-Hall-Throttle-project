# System Architecture

This document describes the high‑level architecture.

## Blocks

- Mechanics (pedal, spring, stops)
- Sensors (A1324 analog, SparkFun 23880 I²C)
- MCU (ATmega32U4, USB HID)
- PC UI (Windows, sidebar modes)

## Data Flow

```text
Mechanical → Raw sensor → Filtering → Calibration → Deadzones
→ Profile curve → Output → USB HID report