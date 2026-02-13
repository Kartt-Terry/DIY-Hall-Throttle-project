# Modes (DIY10_REV1)

## DRIVE
- Oletustila bootissa
- HID aktiivinen (50 Hz)
- Komennot rajattu: IC_START, UC_START, SETTINGS_START, HELP, PING, PRINT_STATUS, PRINT_LIVE, ABORT

## CALIBRATION
- IC/UC suoritetaan täällä
- HID freeze oletuksena (CAL_PREVIEW_HID_ON/OFF mahdollista, throttlaus 10 Hz, max 15 s)
- Auto‑revert: timeout (60 s), yhteyskatko (3 s), virheburski, CANCEL/ABORT

## SETTINGS
- Asetukset & LUT
- HID aktiivinen oletuksena (freeze toggle tarvittaessa)
- LUT chunkkeina → COMMIT_LUT; transfer window 180 s

---

## ASCII tilakone (MODE → MODE)

```
[DRIVE]
   | IC_START / UC_START           | SETTINGS_START
   v                               v
[CALIBRATION]  --(SAVE/CANCEL/TIMEOUT/ABORT)-->  [DRIVE]
[SETTINGS]     --(COMMIT/CANCEL/TIMEOUT/ABORT)--> [DRIVE]
```

### PHASEt (alavaiheet)
- **CALIBRATION:** `CAL_IDLE`, `CAL_CAP_MAX`, `CAL_CAP_MIN`, `CAL_PREVIEW`, `CAL_READY_TO_SAVE`
- **SETTINGS:** `SET_IDLE`, `SET_PARAM_EDIT`, `SET_LUT_TRANSFER`, `SET_COMMIT_PENDING`
