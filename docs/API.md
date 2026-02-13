# Command API — DIY10_REV1

## Vasteformaatti
`OK|WARN|ERR: <msg> ... MODE=<DRIVE|CALIBRATION|SETTINGS> HIDF=<0|1> [PHASE=<...>]`

## Yleiset (kaikissa moodeissa)
- `HELP`, `PING`, `ABORT`, `PRINT_STATUS`, `PRINT_LIVE [rate_hz]` (0=off, oletus 10 Hz)

## DRIVE → CAL/SET
- `IC_START` → MODE=CAL, HIDF=1
- `UC_START` → MODE=CAL, HIDF=1
- `SETTINGS_START` → MODE=SET, HIDF=0

## CALIBRATION (IC/UC)
- `IC_CAPTURE_MAX` / `IC_CAPTURE_MIN` / `IC_PREVIEW` / `IC_SAVE`
- `UC_CAPTURE_MAX` / `UC_CAPTURE_MIN` / `UC_PREVIEW` / `UC_SAVE`
- `CAL_PREVIEW_HID_ON` / `CAL_PREVIEW_HID_OFF` (10 Hz, max 15 s)
- `CANCEL`
- Timeout 60 s, conn lost 3 s → paluu DRIVE

## SETTINGS
- Parametrit:  
  `SET_DZ <permille> (0..200)`  
  `SET_ALPHA <permille> (1..999)`  
  `SET_MISMATCH <delta_thr_permille 5..100> <fault_trigger_ms 20..1000> <fault_clear_ms 50..2000>`  
  `SET_TIMEOUTS <cal_timeout_s 10..600> <set_timeout_s 10..600> <conn_lost_ms 500..10000> <transfer_timeout_s 30..600>`  
  `SET_RATE <drive_hz ∈ {10,20,25,33,40,50}> <cal_preview_hz 5..20>`  
  `SET_NORMALIZE_POLICY <0|1|2>` (0=UC_ONLY, 1=UC_CLAMPED_BY_IC, 2=IC_ONLY_FALLBACK)  
  `SET_AUTO_UC <0|1>`
- HID freeze: `SET_HID_FREEZE_ON` / `SET_HID_FREEZE_OFF`
- LUT:  
  `SET_LUT_IDENTITY`  
  `SET_LUT <idx 0..64> <value 0..10000>` (aktivoi transfer window 180 s)  
  `COMMIT_LUT` (validoi 65 pistettä, kirjoita EEPROM) → paluu DRIVE
- `CANCEL` → paluu DRIVE

## Auto‑UC (UI‑vapaa)
- Bootin jälkeen 3–5 s ikkunassa: **3 selkeää polkaisua** → AUTO_UC_START (sisäinen)
- Keruu 5–8 s → AUTO_UC_SAVE tai abort → DRIVE

## Esimerkit (tiiviisti)

```
IC_START
IC_CAPTURE_MAX
IC_CAPTURE_MIN
IC_PREVIEW
IC_SAVE
```

```
SETTINGS_START
SET_DZ 20
SET_LUT 0 0
...
COMMIT_LUT
```
