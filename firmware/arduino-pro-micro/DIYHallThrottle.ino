// DIY Hall Throttle - Arduino Pro Micro (ATmega32U4)
// Modes: DRIVING, SETTINGS, FAULT, DIAGNOSTICS
// Sensors: A1324 (analog), SparkFun 23880 (I2C)

#include <Arduino.h>
#include "src/config.h"
#include "src/state_machine.h"
#include "src/protocol.h"
#include "src/sensors.h"
#include "src/processing.h"
#include "src/storage.h"
#include "src/usb_hid.h"

static StateMachine g_state;

void setup() {
  Serial.begin(115200);
  delay(50);
  sensors_init();
  storage_init();
  processing_init();
  usb_hid_init();
  g_state.init();
}

void loop() {
  protocol_poll();        // handle incoming JSON commands
  sensors_poll();         // read raw
  processing_step();      // filter → calibrate → deadzones → profile

  if (g_state.mode() == MODE_DRIVING || g_state.mode() == MODE_SETTINGS) {
    usb_hid_send(processing_get_output());
  }

  g_state.update();       // handles faults + boot gesture detection
}
