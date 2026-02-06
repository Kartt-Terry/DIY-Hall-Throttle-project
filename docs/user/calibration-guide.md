# Calibration Guide

**Goal:** Map mechanical travel to the full 0–100% throttle range.

## UI Calibration (Recommended)
1) Release pedal → **Capture Min**
2) Full press → **Capture Max**
3) **Save to Device** (EEPROM)

## No-UI Boot Gesture (Fallback)
- After power-up, press the pedal to **full** three times quickly (within ~5 seconds). 
- Device enters guided calibration: prompts via Serial/LED (if available) to capture MIN then MAX, then saves.

## Expected Results
- Rest → 0.0 ± 0.5 %
- Full → 100.0 ± 0.5 %

## Tips
- Adjust mechanical stops **before** calibration.
- Repeat if hardware changes or temperature drift is observed.
