# USB HID Spec (Throttle Axis)

- Report type: Joystick/Gamepad axis (single axis sufficient)
- Resolution: 10–16 bits (configurable; start with 10 bit to match ADC)
- Rate: 250–1000 Hz target

> Implementation note: On ATmega32U4 you can use libraries such as Arduino Joystick or NicoHood HID. Integrate in `usb_hid.*`.
