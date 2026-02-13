# DIY10_REV1 — DIY Hall Throttle Firmware (Arduino Pro Micro, Dual Hall, HID Rz)

**Tavoite:** Luotettava USB HID ‑kaasupoljin kahdella Hall‑anturilla, selkeä kalibrointi (IC/UC), turvallinen mismatch‑valvonta ja pelikäyttöön optimoitu HID‑syöttö.

## Pääkohdat

- **Alusta:** Arduino Pro Micro (ATmega32U4) → natiivi USB HID
- **Anturit:** 2 × Allegro A1324 (analogiset A0 & A1)
- **HID:** ArduinoJoystickLibrary (Matthew Heironimus), **Variant B**  
  → firmware antaa **0..1023** `setRzAxis()`; kirjasto skaalaa **0..65535**
- **Signaaliketju:**  
  `RAW → IIR (α=0.20) → normalize (EEPROM MIN/MAX) → deadzone (2%) → LUT (65p, 0..10000) → mismatch → OUT → HID Rz`
- **Mismatch:** Δ = |Y1 − Y2|; Fault jos Δ > 3% yli 100 ms; clear 300 ms; Fault‑tilassa OUT=0
- **Mode‑arkkitehtuuri:**  
  - **DRIVE** (oletus): HID aktiivinen, minimi Serial‑liikenne  
  - **CALIBRATION:** IC/UC‑kalibrointi, HID **freeze** oletuksena  
  - **SETTINGS:** asetukset & LUT; HID **aktiivinen oletuksena** (voi jäädyttää komennolla)
- **CSV (Serial Plotter):**  
  `S1,S2,V1,V2,F1,F2,N1,N2,D1,D2,Y1,Y2,DELTA,FAULT,OUT,RZ`  
  (PRINT_LIVE päällä lisätään: `MODE,HIDF,PHASE`)

## Projektitila

- Firmware: **Step 4 + HID — Variant B (0..1023 syöttö)**
- Windowsissa testattu: `joy.cpl` → **Z Rotation** liikkuu tasaisesti 0 ↔ 1023

## Kansiot

```
firmware/
  arduino-pro-micro/
    experiments/
      DIYThrottle_RawCalib/
        DIYThrottle_RawCalib.ino
        README.md
docs/
  OVERVIEW.md
  MODES.md
  CALIBRATION.md
  EEPROM.md
  API.md
  CSV.md
  TROUBLESHOOTING.md
  STATE_MACHINE.md
CHANGELOG.md
CONTRIBUTING.md
```

## Asennus

1. **Arduino IDE** + Board: “Arduino Leonardo / Pro Micro (ATmega32U4)”
2. Asenna **ArduinoJoystickLibrary (Matthew Heironimus)** → *Sketch → Include Library → Add .ZIP Library*
3. Avaa `firmware/.../DIYThrottle_RawCalib/DIYThrottle_RawCalib.ino` ja lataa Pro Microon

> HID‑malli: **Variant B** (kutsu `setRzAxisRange(0,1023)` ja anna `0..1023` arvoja).

## Nopea aloitus

1. Kytke laite → Windows: `Win+R → joy.cpl` → valitse laite → **Test**
2. **IC (Initial Calibration)** UI:n kautta:  
   “Initial Calibration” → ohjeiden mukaan **MAX → MIN → Save**
3. **UC (User Calibration)** UI:n kautta:  
   Sama kuin IC → **Save**  
   **tai** UI‑vapaasti: kytke virta ja polkaise **3 kertaa** → laite kerää min/max 5–8 s ja tallentaa automaattisesti, jos range riittää

## Komento‑API (pikayhteenveto)

- **Yleiset:** `HELP`, `PING`, `ABORT`, `PRINT_STATUS`, `PRINT_LIVE [rate]`
- **DRIVE → CAL/SET:** `IC_START`, `UC_START`, `SETTINGS_START`
- **Calibration (IC/UC):** `*_CAPTURE_MIN`, `*_CAPTURE_MAX`, `*_PREVIEW`, `*_SAVE`, `CAL_PREVIEW_HID_ON/OFF`, `CANCEL`
- **Settings:** `SET_DZ`, `SET_ALPHA`, `SET_MISMATCH`, `SET_TIMEOUTS`, `SET_RATE`, `SET_NORMALIZE_POLICY`, `SET_AUTO_UC`,  
  LUT: `SET_LUT_IDENTITY`, `SET_LUT <idx> <value>`, `COMMIT_LUT`, HID freeze: `SET_HID_FREEZE_ON/OFF`, `CANCEL`

Täysi spesifikaatio: **docs/API.md**

## EEPROM (A/B peilaus, CRC)

- **IC_A/B (48 B):** mekaaniset min/max  
- **UC_A/B (48 B):** käyttömin/max (+source, cycles, quality)  
- **CFG_A/B (192 B):** deadzone, α, mismatch‑rajat, timeoutit, taajuudet, normalize_policy, LUT(65p)  
- **GLOBAL_INFO (32 B)**, **RESERVED (~416 B)**
- Commit aina passiiviseen kopioon (seq++), CRC‑16; Bootissa valitaan validi & uusin

Täysi kuvaus: **docs/EEPROM.md**

## CSV‑telemetria

Perusheader:  
`S1,S2,V1,V2,F1,F2,N1,N2,D1,D2,Y1,Y2,DELTA,FAULT,OUT,RZ`  
`PRINT_LIVE` päällä lisätään `MODE,HIDF,PHASE`. Taajuudet: DRIVE 50 Hz, CAL preview 10 Hz, SETTINGS 50 Hz.

Lisätiedot: **docs/CSV.md**

## Versiointi & nimeäminen

- Firmware‑nimi: **DIY10_REV1** (ensimmäinen virallinen revisio)

## Kiitokset

- **ArduinoJoystickLibrary** — Matthew Heironimus  
- Yhteisön ideat LUT‑käsittelyyn ja HID‑profiileihin

## Lisenssi

MIT (tai halutessasi muu lisenssi). Katso `LICENSE` kun lisätty.
