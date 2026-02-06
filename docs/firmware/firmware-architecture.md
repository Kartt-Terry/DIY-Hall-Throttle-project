# Firmware Architecture

- State machine: DRIVING, SETTINGS, FAULT, DIAGNOSTICS
- Sensor backends: analog (A1324) and I²C (SparkFun 23880)
- Processing: filtering → calibration → deadzones → profile mapping → HID
- EEPROM: persistent config
- Serial protocol: simple JSON commands from UI
