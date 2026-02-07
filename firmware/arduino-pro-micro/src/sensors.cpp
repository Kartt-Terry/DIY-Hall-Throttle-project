#include "sensors.h"
#include "config.h"
#include <Arduino.h>
#include <Wire.h>

static int  s_raw         = 0;
static bool s_i2c_present = false;
static bool s_ok          = true;

void sensors_init() {
  pinMode(PIN_HALL_ANALOG, INPUT);
  Wire.begin();
  // Quick I2C probe (placeholder address)
  Wire.beginTransmission(I2C_ADDR_23880);
  s_i2c_present = (Wire.endTransmission() == 0);
}

void sensors_poll() {
  // CI: käytetään aina analogista A1324-luentaa. I2C jätetään toistaiseksi pois.
  s_raw = analogRead(PIN_HALL_ANALOG); // 0..1023

  // Jos haluat ottaa I2C:n käyttöön myöhemmin, tee se tähän:
  // if (s_i2c_present) {
  //   // TODO: lue SparkFun 23880 ja map 0..1023
  //   // s_raw = mapped_value;
  // }
}

int  sensors_get_raw()      { return s_raw; }
bool sensors_detect_i2c()   { return s_i2c_present; }
bool sensors_ok()           { return s_ok; }