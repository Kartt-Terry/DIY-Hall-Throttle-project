
# Signal Processing Summary

This document summarizes the signal processing architecture for the dual A1324 Hall-sensor throttle pedal using the Arduino Pro Micro platform. It covers the complete signal chain, LUT-based profiling, dual-sensor fusion, mismatch detection, traffic policy (Driving vs. Diagnostics), timing, EEPROM storage, and testing.

---

## 1. Signal Processing Pipeline — Step by Step

For each sensor channel (1 and 2):

1. **ADC Sample**  
   - `raw ∈ [0…1023]` (10-bit ADC, AVCC reference).  
   - Diagnostic voltages:  
     `S1_V = raw1/1023 * Vsys`, `S2_V = raw2/1023 * Vsys`  
     where `Vsys ≈ 5.00 V` or measured via AVR bandgap.

2. **IIR Filtering**  
   `f = f + α*(raw − f)`; default **α = 0.2** (UI-adjustable later).

3. **Calibration → Normalization**  
   `n = clamp((f − MIN) / (MAX − MIN), 0…1)`  
   (MIN/MAX values per sensor loaded from EEPROM).

4. **Deadzone Correction**  
   - If `n < dead_low` → 0  
   - If `n > 1 − dead_high` → 1  
   - Default: `dead_low = dead_high = 0.02`.

5. **Profile: LUT (65 points, linear interpolation)**  
   - 65 points, indices **0..64**.  
   - Linear interpolation:  
     `idx = n*64`, `i = floor(idx)`, `frac = idx − i`,  
     `y = (1−frac)*LUT[i] + frac*LUT[i+1]`.  
   - Supported profiles: **Linear, Expo, Log, LUT** (primary option: LUT).

### Dual-Sensor Fusion and Quality

6. **Fusion**:  
   `OUT = (y1 + y2) / 2` (default: average fusion).

7. **Mismatch Detection**:  
   `Δ = |y1 − y2|`.  
   If `Δ > 0.03` (3%) **continuously** (≈75–100 ms), → **FAULT**.

8. **HID Output**:  
   `OUT` mapped to USB HID axis 0–100% (or 0–1023/65535 depending on report format).

---

## 2. LUT Profile (65 Points, Linear Interpolation)

- **65 points** (index range 0..64) — optimal balance of shaping flexibility and CPU efficiency.  
- Stored in EEPROM as **uint16** values (0..10000).  
- Converted to 0..1 range in RAM.  
- **Linear interpolation** ensures smooth continuous response.  
- **UI presets:** Linear, Expo 1.4, Expo 1.8, Log Gentle, S-curve.  
- **UI Smooth function:** 1–3 passes of moving average smoothing (with optional monotonicity enforcement).

---

## 3. Calibration — Two Methods

### 3.1 Boot Gesture (No UI)
- After power-on, firmware monitors pedal for ~5 seconds.  
- User presses pedal **fully three times quickly** → triggers calibration mode.  
- MAX capture: hold pedal fully down → read MAX1/MAX2 (median of several samples).  
- MIN capture: release pedal → read MIN1/MIN2.  
- Validate (`MAX > MIN + margin`) → save to EEPROM → return to DRIVING.

### 3.2 UI-Controlled Calibration
- User clicks *Start Calibration* in UI.  
- Modal steps: **Capture MAX** → **Capture MIN** → **Save**.  
- Stores MIN1/MAX1/MIN2/MAX2 to EEPROM.

**Note:** Always adjust mechanical stops **before** calibration; calibration can be repeated freely.

---

## 4. Modes and Traffic Policy (Minimal Communication Design)

- **DRIVING Mode:**  
  - No diagnostic telemetry sent to UI.  
  - Only HID output (optional heartbeat 1–2 Hz).

- **DIAGNOSTICS Mode:**  
  - Full telemetry 10–20 Hz:  
    `v_sys, s1_v, s2_v, raw1/2, f1/2, n1/2, y1/2, out, delta, profile, mode`.

---

## 5. Timing and Performance

- **Internal processing:** 500–1000 Hz (ADC + IIR + LUT + fusion).  
- **Diagnostics telemetry:** 10–20 Hz (UI-friendly).  
- **CPU load:** IIR + LUT interpolation is extremely light for ATmega32U4.

---

## 6. EEPROM & Configuration

Stored parameters include:
- MIN1, MAX1, MIN2, MAX2  
- dead_low, dead_high  
- profile_type (0=Linear, 1=Expo, 2=Log, 3=LUT)  
- expo_k (for expo profiles)  
- LUT[65] (uint16 values 0..10000)  
- CRC16 checksum

---

## 7. Testing Recommendations

- Identity LUT (Linear) → OUT ≈ n across full travel.  
- Presets → Expo/Log/S-curve match preview and respect endpoints.  
- Smooth → reduces abrupt jumps; values remain in [0..1].  
- Mismatch → inject y1≠y2 → Δ exceeds threshold → FAULT (HID=0%).  
- Persistence → Save → power cycle → configuration persists.  
- Traffic → Driving silent; Diagnostics interactive 10–20 Hz.

---

## 8. Benefits to User and Developer

- LUT enables fine-grained response shaping with presets and smoothing.  
- Dual-sensor averaging improves stability; mismatch detection increases safety.  
- HID output stays responsive, with minimal serial traffic during driving.  
- Diagnostics mode gives full visibility for tuning and debugging.
