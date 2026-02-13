# STATE MACHINE — DIY10_REV1

Tämä dokumentti määrittelee DIY10_REV1‑firmwaren tilakoneen (MODE/PHASE), tapahtumat (events), siirtymät (transitions), HID‑politiikan, timeoutit ja sekvenssit IC/UC‑kalibroinneille (UI & auto‑UC). Tämä on toteutusta ohjaava arkkitehtuurispesifikaatio (ei koodia).

---

## 1) Top-level MODEt

- **DRIVE** — pelikäyttö (oletus)
  - HID aktiivinen, minimi Serial‑melu
  - Whitelist: `IC_START, UC_START, SETTINGS_START, HELP, PING, PRINT_STATUS, PRINT_LIVE, ABORT`

- **CALIBRATION** — IC/UC‑kalibrointi
  - HID **freeze** oletuksena (poikkeus: preview throttlattuna)
  - Whitelist: IC/UC‑komennot + `CAL_PREVIEW_HID_ON/OFF, PRINT_STATUS, PRINT_LIVE, CANCEL, ABORT`

- **SETTINGS** — asetukset, LUT, diagnostiikka
  - HID **aktiivinen oletuksena**
  - Whitelist: `SET_*`, `SET_LUT*`, `COMMIT_LUT`, `SET_HID_FREEZE_ON/OFF`, `PRINT_STATUS`, `PRINT_LIVE`, `CANCEL`, `ABORT`

**Auto‑revert kaikista ei‑DRIVE‑tiloista:** timeout, yhteyskatko, virheburski, `CANCEL`/`ABORT` → paluu DRIVE.

---

## 2) ASCII: MODE‑siirtymät

```
+---------+         IC_START / UC_START         +---------------+
|  DRIVE  |  ---------------------------------> |  CALIBRATION  |
| (default|                                      +---------------+
|  @boot) |-- SETTINGS_START ----------------->  |   SETTINGS    |
+---------+                                      +---------------+

CALIBRATION --(SAVE / CANCEL / TIMEOUT / ABORT / CONN_LOST / ERROR_BURST)--> DRIVE
SETTINGS   --(COMMIT / CANCEL / TIMEOUT / ABORT / CONN_LOST / ERROR_BURST)--> DRIVE
```

---

## 3) PHASE‑alatilat

### 3.1 CALIBRATION PHASE
- `CAL_IDLE` — odottaa capture‑komentoa
- `CAL_CAP_MAX` — stabilointi + mittaus MAX
- `CAL_CAP_MIN` — stabilointi + mittaus MIN
- `CAL_PREVIEW` — välitulosten esittely
- `CAL_READY_TO_SAVE` — validointi OK, valmiina tallentamaan

### 3.2 SETTINGS PHASE
- `SET_IDLE` — ei aktiivista muokkausta
- `SET_PARAM_EDIT` — parametrisäätö (DZ, α, mismatch, taajuudet, timeoutit, normalize policy, auto‑UC)
- `SET_LUT_TRANSFER` — LUT chunk‑siirto (aktivoi transfer window)
- `SET_COMMIT_PENDING` — valmis commitille (valinnainen välitila)

---

## 4) HID‑politiikka

- **DRIVE:** `HIDF=0` (HID aktiivinen @ 50 Hz)
- **CALIBRATION:** `HIDF=1` oletuksena  
  - `CAL_PREVIEW_HID_ON` → `HIDF=0` **throttlaus 10 Hz**, **max 15 s** (auto OFF)
  - `CAL_PREVIEW_HID_OFF` → `HIDF=1`
- **SETTINGS:** `HIDF=0` oletuksena  
  - `SET_HID_FREEZE_ON` / `SET_HID_FREEZE_OFF` → manuaalinen toggle (esim. LUT‑commitin ajaksi)

**Freeze‑toteutus:** kun `HIDF=1`, älä lähetä uusia `setRzAxis()` päivityksiä → Windows pitää viimeisimmän arvon.

---

## 5) Timeoutit & yhteyskatkot

- `cal_timeout_s` = **60 s** (oletus)  
- `set_timeout_s` = **60 s** (oletus)  
- `conn_lost_ms` = **3000 ms** (ei komentoja, ei siirtoa → revert)  
- `transfer_timeout_s` = **180 s** (LUT‑siirtoikkuna)  
- `errorBurst` = ≥3 `ERR:` / 10 s → revert

Kaikki arvot konfiguroitavissa **CFG‑lohkossa**.

---

## 6) Event → Transition (MODE‑tasolla)

### DRIVE → CALIBRATION
- **Event:** `IC_START` **tai** `UC_START`
- **Guard:** ei toista CAL/SET prosessia
- **Action:** `MODE=CALIBRATION`, `HIDF=1`, `PHASE=CAL_IDLE`, start `cal_timeout_s`
- **Exit:** banner `MODE=CALIBRATION HIDF=1 REASON=<IC_START|UC_START>`

### DRIVE → SETTINGS
- **Event:** `SETTINGS_START`
- **Action:** `MODE=SETTINGS`, `HIDF=0`, `PHASE=SET_IDLE`, start `set_timeout_s`
- **Exit:** banner `MODE=SETTINGS HIDF=0 REASON=USER_REQUEST`

### CALIBRATION → DRIVE
- **Event:** `IC_SAVE` / `UC_SAVE` / `CANCEL` / `TIMEOUT` / `CONN_LOST` / `ERROR_BURST` / `ABORT`
- **Action:** (mahdollinen commit), stop timers, `HIDF=0`
- **Exit:** banner `RETURNING_TO_DRIVE HIDF=0 REASON=<...>`

### SETTINGS → DRIVE
- **Event:** `COMMIT_LUT` / `CANCEL` / `TIMEOUT` / `CONN_LOST` / `ERROR_BURST` / `ABORT`
- **Action:** (mahdollinen commit), stop timers, `HIDF=0`
- **Exit:** banner `RETURNING_TO_DRIVE HIDF=0 REASON=<...>`

---

## 7) Event → Transition (PHASE‑tasolla, CAL)

### CAPTURE MAX
- **Event:** `IC_CAPTURE_MAX` **tai** `UC_CAPTURE_MAX`
- **Guard:** `PHASE in {CAL_IDLE, CAL_PREVIEW}` (ei jo capturea käynnissä)
- **Action:**  
  - `PHASE=CAL_CAP_MAX`  
  - Stabilointi: ~200 ms **tai** 4 peräkkäistä sykliä |dV/dt| < raja  
  - Mittaus: 32 näytettä @ 50 Hz → **mediaani** (tai “median of means”)  
  - Δ‑tarkistus: jos >3 %, `WARN`  
  - Talleta MAX väliaikaiseen bufferiin  
  - `PHASE=CAL_PREVIEW`  
- **Error:**  
  - `ERR: RANGE_TOO_SMALL` kun min/max erotus ei toteudu  
- **Vaste:** `OK: IC_CAPTURE_MAX ... MODE=CALIBRATION HIDF=1 PHASE=CAL_PREVIEW`

### CAPTURE MIN
- **Event:** `IC_CAPTURE_MIN` **tai** `UC_CAPTURE_MIN`
- **Sama** kuin MAX, mutta MIN suuntaan

### PREVIEW
- **Event:** `IC_PREVIEW` / `UC_PREVIEW`
- **Guard:** vähintään toisen (MIN/MAX) tulee olla mitattu
- **Action:** arvioi validi range; jos molemmat mitattu ja ok → `PHASE=CAL_READY_TO_SAVE`
- **Vaste:** `OK: IC_PREVIEW ... RANGE_OK=TRUE MODE=CALIBRATION HIDF=1 PHASE=CAL_READY_TO_SAVE`

### SAVE
- **Event:** `IC_SAVE` / `UC_SAVE`
- **Guard:** `PHASE=CAL_READY_TO_SAVE` & validit range‑arvot
- **Action:** EEPROM commit (A/B, seq++), banner, timers off
- **Transition:** `→ DRIVE`

---

## 8) Event → Transition (PHASE‑tasolla, SET)

### Parametrien asetus
- **Event:** `SET_DZ`, `SET_ALPHA`, `SET_MISMATCH`, `SET_TIMEOUTS`, `SET_RATE`, `SET_NORMALIZE_POLICY`, `SET_AUTO_UC`
- **Action:** validoi, kirjoita suoraan **CFG A/B** (seq++, CRC)
- **PHASE:** `SET_PARAM_EDIT`

### LUT siirto
- **Event:** `SET_LUT <idx> <value>`
- **Action:** aloita siirtoikkuna (180 s) jos ei jo käynnissä; päivitä RAM‑puskuri; progress
- **PHASE:** `SET_LUT_TRANSFER`

### LUT commit
- **Event:** `COMMIT_LUT`
- **Guard:** 65 pistettä valmiina & validi alue
- **Action:** kirjoita **CFG A/B**, banner
- **Transition:** `→ DRIVE`

---

## 9) Serial‑UX ja CSV

**Vasteformaatti:**  
`OK|WARN|ERR: <msg> ... MODE=<DRIVE|CALIBRATION|SETTINGS> HIDF=<0|1> [PHASE=<...>]`

**Tilabannerit:** aina siirtymissä:
- `MODE=DRIVE HIDF=0 REASON=BOOT`
- `MODE=CALIBRATION HIDF=1 REASON=IC_START`
- `MODE=SETTINGS HIDF=0 REASON=USER_REQUEST`
- `RETURNING_TO_DRIVE HIDF=0 REASON=<...>`

**CSV‑tail (kun `PRINT_LIVE`):** lisää `MODE,HIDF,PHASE` loppuun.

---

## 10) IC‑sekvenssi (UI)

```
UI: "Initial Calibration" → IC_START
FW: MODE=CALIBRATION HIDF=1 PHASE=CAL_IDLE
UI: "Paina pohjaan" → OK → IC_CAPTURE_MAX
FW: stabilointi+32 näytettä → OK: IC_CAPTURE_MAX ... PHASE=CAL_PREVIEW
UI: "Päästä ylös" → OK → IC_CAPTURE_MIN
FW: stabilointi+32 näytettä → OK: IC_CAPTURE_MIN ... PHASE=CAL_READY_TO_SAVE
UI: IC_PREVIEW → OK: ... RANGE_OK=TRUE PHASE=CAL_READY_TO_SAVE
UI: Save → IC_SAVE
FW: EEPROM commit → RETURNING_TO_DRIVE HIDF=0 REASON=SAVE_OK
```

---

## 11) UC‑sekvenssi (UI)

```
UI: "User Calibration" → UC_START
FW: MODE=CALIBRATION HIDF=1 PHASE=CAL_IDLE
UI: "Ajossa käytettävä max" → OK → UC_CAPTURE_MAX
FW: stabilointi+32 näytettä → OK ... PHASE=CAL_PREVIEW
UI: "Ajossa luonnollinen min" → OK → UC_CAPTURE_MIN
FW: stabilointi+32 näytettä → OK ... PHASE=CAL_READY_TO_SAVE
UI: UC_PREVIEW → OK ... RANGE_OK=TRUE
UI: Save → UC_SAVE
FW: EEPROM commit → RETURNING_TO_DRIVE HIDF=0 REASON=SAVE_OK
```

---

## 12) UC‑sekvenssi (UI‑vapaa, “3 polkaisua”)

```
BOOT → MODE=DRIVE
FW: 3–5 s ikkunassa tunnista 3 selkeää polkaisusykliä
FW: AUTO_UC_START → MODE=CALIBRATION HIDF=1
FW: 5–8 s dynaaminen min/max keruu (IIR/median suodatettuna)
FW: jos range ≥ kynnys → AUTO_UC_SAVE (EEPROM commit UC) → RETURNING_TO_DRIVE
    muuten → ERR: AUTO_UC_RANGE_TOO_SMALL → RETURNING_TO_DRIVE
```

---

## 13) Guardit, virheet, varoitukset (yhteenveto)

**Guard‑esimerkkejä**
- `CMD_NOT_ALLOWED_IN_<MODE>`: komento ei kuulu whitelistaan
- `INVALID_SEQUENCE`: SAVE ilman validia PREVIEW vaihetta
- `OUT_OF_RANGE`: parametri tai LUT‑piste virheellinen
- `RANGE_TOO_SMALL`: (max−min) < kynnys (esim. 40 LSB)
- `MIN_GE_MAX`: min ≥ max
- `CRC_MISMATCH` / `EEPROM_WRITE_FAIL`: persistenssivirheet
- `TIMEOUT_IN_<MODE>` / `CONN_LOST` / `ERROR_BURST_PROTECTION`

**Varoitukset**
- `MISMATCH_DELTA <x.x>%` kalibroinnin aikana (ei estä tallennusta)
- `LUT_TRANSFER_IDLE` jos chunkkien välissä pitkä tauko (ei vielä timeout)

---

## 14) Testaus‑checklist (MODE/PHASE)

- Boot → `MODE=DRIVE HIDF=0`
- DRIVE → IC_START → CAL (`HIDF=1`) → CAP_MAX/MIN → PREVIEW → SAVE → DRIVE
- UC (UI) identtinen polku
- UC (auto) → 3 polkaisua, dynaaminen keruu, ehto: onnistuu/hylkää → DRIVE
- SETTINGS → SET_* parametrit → commit heti CFG:hen (A/B)
- LUT: SET_LUT chunkkeina → COMMIT_LUT → DRIVE; timeout hylkää
- HID: freeze‑politiikka toteutuu; CAL preview throttlaus 10 Hz, auto‑off 15 s
- Timeoutit, conn_lost, abort → aina paluu DRIVE
- CSV: `MODE,HIDF,PHASE` tail vain `PRINT_LIVE` päällä
