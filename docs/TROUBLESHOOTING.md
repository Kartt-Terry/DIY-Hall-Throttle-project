# Troubleshooting

## Z Rotation ei liiku Windowsissa
- Varmista, että käytössä on **Variant B** (firmware antaa 0..1023; kirjasto skaalaa 0..65535)
- Et ole CALIBRATION‑tilassa HID freeze päällä (tarvittaessa `CAL_PREVIEW_HID_ON`)

## IC/UC epäonnistuu
- `ERR: RANGE_TOO_SMALL` → polje suuremmalla amplitudilla tai säädä stoppeja
- `ERR: MIN_GE_MAX` → suorita kalibrointi uudelleen oikeassa järjestyksessä
- Δ > 3 % → `WARN` (ei estä tallennusta, mutta tarkista magneetin linjaus)

## Auto‑UC ei käynnisty
- Tee **3 selkeää polkaisua** 3–5 s sisällä boottauksen jälkeen
- `SET_AUTO_UC 1` on päällä (Settings)

## LUT‑päivitys ei tallennu
- Muista `COMMIT_LUT` → vasta tämä kirjoittaa EEPROMiin
- Transfer window aikakatkaisu 180 s → aloita siirto alusta

## Yhteys katkeaa kalibroinnissa
- Firmware palaa automaattisesti **DRIVE**en; aloita kalibrointi alusta
