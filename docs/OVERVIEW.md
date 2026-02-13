# DIY10_REV1 — Overview

## Arkkitehtuuri (lyhyesti)
- **MODEt:**  
  - **DRIVE:** pelikäyttö, HID aktiivinen, vähäinen Serial‑melu  
  - **CALIBRATION:** IC/UC; HID freeze oletuksena; auto‑revert virheissä  
  - **SETTINGS:** asetukset & LUT; HID aktiivinen oletuksena (toggle freeze)
- **Signaaliputki:**  
  `RAW → IIR (α=0.20) → normalize (UC/IC) → deadzone (2%) → LUT (65p, 0..10000) → mismatch → OUT → HID Rz`
- **HID:** Variant B → firmware 0..1023 → kirjasto 0..65535

## Turvallisuus
- Kaksi Hall‑anturia → mismatch‑valvonta (Δ>3% → fault 100 ms; clear 300 ms; faultissa OUT=0)
- Kalibroinnissa HID freeze → peli ei näe siirtymäkohinaa
- Auto‑revert CAL/SET → aina paluu DRIVE, jos virhe/timeout/yhteyskatko

## Kalibroinnit
- **IC (Initial Calibration):** mekaaninen totuus (ääriarvot), UI‑ohjattu MAX→MIN→Save
- **UC (User Calibration):** käyttöliikerata (ensisijainen normalisointiin), UI‑ohjattu tai UI‑vapaa **auto‑UC (3 polkaisua bootissa)**

## EEPROM
- IC/UC/CFG A/B‑peilaus, CRC‑16, seq‑laskuri, rollback‑turvallinen
- Normalisointipolitiikka: 0=UC_ONLY (suositus), 1=UC_CLAMPED_BY_IC, 2=IC_ONLY_FALLBACK
