# Firmware (Arduino Pro Micro)

This folder holds the production firmware and experiments for the DIY Hall throttle.

## Structure

```
firmware/
  arduino-pro-micro/
    src/                    # shared/primary source files
    versions/
      v0.1.0/              # tagged, immutable release snapshot
    experiments/           # try-outs, spikes, quick tests
```

## Versioning workflow

1. Develop in `src/`.
2. When the code is ready for a release, **copy** the exact sources into `versions/vX.Y.Z/` and commit.
3. Create a Git tag `vX.Y.Z` pointing to that commit.
4. Use GitHub Releases to attach compiled artifacts (e.g., `.hex`) and release notes.

## How to build (Arduino IDE)

- **Board:** Arduino AVR Boards → *Arduino Leonardo* (Pro Micro compatible)
- **Port:** Select COM port of your Pro Micro
- **Baud:** 115200 for Serial Monitor/Plotter

## Quick commands (Serial Monitor)

- `?` → help
- `x` → capture **MAX** (press fully, then send `x`)
- `n` → capture **MIN** (release fully, then send `n`)
- `s` → save to EEPROM
- `p` → print MIN/MAX
- `r` → reset config

> Diagnostics stream is sent **only** in Diagnostics mode; Driving mode keeps serial traffic minimal.
