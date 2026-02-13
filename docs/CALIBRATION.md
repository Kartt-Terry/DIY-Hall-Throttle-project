# Calibration — IC & UC

## IC — Initial Calibration (UI)
1. UI: “Initial Calibration” → lähettää `IC_START` → MODE=CAL, HID freeze
2. UI ohjeistaa: **paina pohjaan** → OK → `IC_CAPTURE_MAX`
3. UI ohjeistaa: **päästä ylös** → OK → `IC_CAPTURE_MIN`
4. `IC_PREVIEW` → jos ok → **Save** (`IC_SAVE`) → paluu DRIVE
- Mittaus: stabilointi ~200 ms + 32 näytettä (mediaani), Δ‑varoitus jos >3 %

## UC — User Calibration (UI)
- Sama virta kuin IC: `UC_START` → MAX/MIN → PREVIEW → `UC_SAVE`

## UC — UI‑vapaa (auto‑UC “3 polkaisua”)
- Bootissa 3–5 s sisällä havaitaan **3 selkeää polkaisusykliä**
- MODE=CAL, HID freeze; kerää min/max **5–8 s**
- Jos (max−min) ≥ kynnys (esim. 40 LSB) → EEPROM UC commit → paluu DRIVE
- Muutoin abort → paluu DRIVE

## Validoinnit
- `min < max`
- `(max−min) ≥ kynnys` (esim. 40 LSB)
- Δ > 3 % → `WARN` (ei estä tallennusta)

## Normalisointi
- **0 = UC_ONLY (suositus):** käytä UC MIN/MAX; fallback IC; viime kädessä 0..1023
- **1 = UC_CLAMPED_BY_IC:** UC klipataan IC‑rajoihin
- **2 = IC_ONLY_FALLBACK**
