#include "sensors.h"
#include "config.h"
#include <Wire.h>

static int s_raw = 0;
static bool s_i2c_present = false;
static bool s_ok = true;

void sensors_init() {
  pinMode(PIN_HALL_ANALOG, INPUT);
  Wire.begin();
  // Quick I2C probe (placeholder address)
  Wire.beginTransmission(I2C_ADDR_23880);
  s_i2c_present = (Wire.endTransmission() == 0);
}

void sensors_poll() {
  // Default: analog read
  s_raw = analogRead(PIN_HALL_ANALOG); // 0..1023

  // TODO: if s_i2c_present, read I2C sensor instead and map to 0..1023
}

int sensors_get_raw() { return s_raw; }

bool sensors_detect_i2c() { return s_i2c_present; }

bool sensors_ok() { return s_ok; }
