# DIY Hall Throttle Project

A DIY throttle pedal system using Hall sensors for precise, contactless position measurement. Includes firmware for Arduino Pro Micro (or similar), a Windows configuration UI (with sidebar navigation) for tuning parameters and full calibration, and comprehensive docs and tests. Designed for use as a high‑precision **sim racing throttle**.

## Key Features
- **Sensors:** A1324 (analog) and SparkFun 23880 (I²C) initial support
- **MCU:** Arduino Pro Micro (ATmega32U4) — USB HID throttle axis
- **UI (Windows):** Sidebar navigation with four modes: Driving, Settings, Fault, Diagnostics
- **Calibration:**
  - From UI (capture MIN/MAX, save to EEPROM)
  - **No‑UI boot gesture:** press pedal to full **three times** quickly after power‑on to enter guided calibration
- **Profiles & Deadzones:** Linear / Exponential / Logarithmic + low/high deadzones
- **Docs & Tests:** Full documentation and multi‑level testing (unit, HIL, E2E, measurements)

## Repository Layout
```

DIY-Hall-Throttle-Project/
├── mechanics/
│   ├── cad/
│   └── drawings/
├── electronics/
│   ├── schematics/
│   ├── pcb/
│   └── sensors/
│       ├── A1324/
│       └── SparkFun-23880/
├── firmware/
│   ├── arduino-pro-micro/
│   │   ├── src/
│   │   ├── tests/
│   │   └── DIYHallThrottle.ino
│   └── tools/
│       └── arduino-cli/
├── software-ui/
│   └── windows/
│       └── ThrottleUI/
│           ├── src/
│           │   ├── Views/
│           │   ├── ViewModels/
│           │   ├── Models/
│           │   └── Services/
│           ├── tests/
│           └── assets/
├── docs/
│   ├── overview/
│   ├── user/
│   ├── mechanics/
│   ├── electronics/
│   ├── firmware/
│   ├── ui/
│   └── quality/
├── tests/
│   ├── hil/
│   ├── measurements/
│   ├── e2e/
│   └── fixtures/
├── .github/
│   ├── workflows/
│   └── ISSUE_TEMPLATE/
├── .gitignore
└── README.md

```

## Quick Start
1. **Firmware**
   - Open `firmware/arduino-pro-micro/DIYHallThrottle.ino` in Arduino IDE or Arduino CLI.
   - Select board: *Arduino Leonardo / ATmega32U4 (Pro Micro compatible).* 
   - Flash and open Serial Monitor (115200) to see logs.
2. **UI (Windows, .NET 8 WPF)**
   - Install .NET 8 SDK
   - `cd software-ui/windows/ThrottleUI`
   - `dotnet build && dotnet run`
3. **Docs**
   - See `docs/user/quick-start.md` and `docs/user/calibration-guide.md`.

## License
MIT — see `LICENSE`.
