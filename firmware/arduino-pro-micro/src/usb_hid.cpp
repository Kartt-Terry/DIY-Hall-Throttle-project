#include "usb_hid.h"
#include <Arduino.h>

void usb_hid_init() {
  // TODO: integrate HID library (e.g., Arduino Joystick or NicoHood HID)
}

void usb_hid_send(float value01) {
  // TODO: send as HID throttle axis; placeholder prints to Serial for now
  Serial.print("OUT:"); Serial.println(value01, 3);
}
