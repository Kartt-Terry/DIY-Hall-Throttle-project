# DIYThrottle_RawCalib Sketch

Open this folder in **Arduino IDE** (File → Open → select this folder).

- **Board:** Arduino Leonardo (for Pro Micro)
- **Baud:** 115200
- **Pins:** A0 = Sensor1, A1 = Sensor2

**Serial commands** (set line ending to Newline):
- `?` help
- `x` capture MAX (hold fully down, then press `x`)
- `n` capture MIN (release fully, then press `n`)
- `s` save to EEPROM
- `p` print MIN/MAX
- `r` reset config

The sketch prints `S1`, `S2`, `V1`, `V2` labels every ~20 ms, so it works with **Serial Plotter**.
