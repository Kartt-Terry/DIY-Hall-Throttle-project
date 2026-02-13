# EEPROM — A/B peilaus, CRC‑16, seq

## Layout (ATmega32U4, 1024 B)

| Offset | Koko | Lohko      |
|------:|----:|-------------|
| 0x0000 | 32  | GLOBAL_INFO |
| 0x0020 | 48  | IC_A        |
| 0x0050 | 48  | IC_B        |
| 0x0080 | 48  | UC_A        |
| 0x00B0 | 48  | UC_B        |
| 0x00E0 | 192 | CFG_A       |
| 0x01A0 | 192 | CFG_B       |
| 0x0260 | 416 | RESERVED    |

## Yhteinen header (kaikissa IC/UC/CFG-lohkoissa)

- `magic` (‘ICB1’ / ‘UCB1’ / ‘CFGB’), `schema`, `size`, `seq`, `stamp`, `flags`  
- CRC16‑CCITT‑FALSE (poly 0x1021, init 0xFFFF, ei xorout)

## IC (48 B/kopio)
- Payload: `s1_min_raw, s1_max_raw, s2_min_raw, s2_max_raw, last_delta_permille, reserved`

## UC (48 B/kopio)
- Payload: `s1_min_raw, s1_max_raw, s2_min_raw, s2_max_raw, source(UI/AUTO), cycles, quality, reserved`

## CFG (192 B/kopio)
- Parametrit: `deadzone_permille, alpha_permille, delta_thr_permille, fault_trigger_ms, fault_clear_ms, drive_rate_hz, cal_preview_rate_hz, cal_timeout_s, set_timeout_s, conn_lost_ms, transfer_timeout_s, normalize_policy, cfg_flags`
- LUT: `lut_points[65]` (0..10000)
- Varaukset myöhempään käyttöön

## Commit‑politiikka
- Kirjoita aina **passiiviseen kopioon** (pienempi `seq`/invalidi), `seq++`, CRC
- Bootissa valitaan **validi + suurin `seq`**
- Molemmat invalidit → **oletusarvot** (LUT=identity, järkevät parametrit)

## Oletusarvot (kun kaikki invalidia)
- Deadzone 2 %, α=0.20, mismatch 3%/100/300 ms, DRIVE=50 Hz, CAL preview=10 Hz,
  timeoutit 60 s / 60 s / 3000 ms / 180 s, normalize_policy=0, LUT=identity
