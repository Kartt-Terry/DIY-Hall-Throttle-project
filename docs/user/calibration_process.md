
# Calibration Process Summary

This document describes the calibration process for the Arduino Pro Micro–based dual‑Hall‑sensor throttle pedal, using two A1324 linear Hall sensors. The system supports two calibration modes: one without a user interface (UI) and one initiated through the Windows UI.

---

## 1. Overview
The throttle pedal provides an axis input to Windows as a USB HID game controller.
To correctly map the pedal’s mechanical travel to a clean 0–100% HID output, the user performs a calibration procedure that records **MIN** and **MAX** values for both A1324 sensors.

Calibration must be performed after setting the mechanical travel limits (stops).

---

## 2. Two Calibration Methods

### 2.1 Calibration Without UI (Boot Gesture)
This “offline” calibration method works immediately after powering the device.

- After power‑up, the firmware monitors pedal activity for ~5 seconds.
- If the user presses the pedal **fully three times quickly**, the firmware interprets this as a calibration gesture.
- Firmware enters **Calibration Mode**.

The process:

1. **MAX capture**: User presses the pedal fully down → firmware records MAX values from both sensors.
2. **MIN capture**: User releases the pedal completely → firmware records MIN values.
3. **Validation**: Firmware checks that `MAX > MIN + safety margin`.
4. **Storage**: Valid MIN/MAX values are written to EEPROM.
5. **Completion**: Firmware returns to **DRIVING** mode.

---

### 2.2 Calibration Through the UI
This method is user‑guided with visual prompts in the Windows application.

- The UI provides a **“Start Calibration”** toggle/button that sends a command to the firmware to enter calibration mode.
- The UI displays modal dialogs guiding the two‑stage process:

#### Step A — Capture MAX
- Prompt: **“Press the pedal fully and click OK.”**
- UI sends `calibrate_max` → firmware records MAX values.

#### Step B — Capture MIN
- Prompt: **“Release the pedal fully and click OK.”**
- UI sends `calibrate_min` → firmware records MIN values.

#### Saving
- UI asks: **“Save to device?”**
- UI sends `save`, which triggers EEPROM write of all calibration data.

- UI closes the modal window and returns to normal operation.

---

## 3. Common Requirements

- User must adjust mechanical stops **before** calibration.
- Calibration can be repeated at any time.
- EEPROM preserves calibration across power cycles.
- Firmware uses MIN/MAX from both sensors to form the final filtered HID output.

---

## 4. Stored Calibration Parameters

- **MIN1**, **MAX1** — sensor 1
- **MIN2**, **MAX2** — sensor 2

Additional optional parameters:
- Deadzone low/high
- Profile type (linear / exponential / logarithmic)
- Exponential curve factor

---

## 5. Fail‑Safe Behavior

- If calibration fails (e.g., `MAX <= MIN`), firmware enters **FAULT** mode.
- In FAULT mode, HID output is forced to **0%** for safety.

---

## Purpose
This document defines the baseline logic required to implement both calibration workflows in the firmware and UI.
