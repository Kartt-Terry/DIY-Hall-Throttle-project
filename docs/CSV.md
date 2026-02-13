# CSV — Serial Plotter

## Perusheader
S1,S2,V1,V2,F1,F2,N1,N2,D1,D2,Y1,Y2,DELTA,FAULT,OUT,RZ

## Debug‑lisäkentät (vain kun PRINT_LIVE päällä)
MODE,HIDF,PHASE

## Taajuudet
- DRIVE: 50 Hz
- CAL preview: 10 Hz (vain `CAL_PREVIEW_HID_ON`)
- SETTINGS: 50 Hz (oletus)

## Käyttövinkit
- Pidä `PRINT_LIVE` maltillisella taajuudella pelikäytön aikana (10–20 Hz), jotta USB/Serial ei lisää viivettä.
