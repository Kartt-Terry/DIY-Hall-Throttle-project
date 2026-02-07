void sensors_poll() {
  // CI: käytä analogista A1324-luentaa, ei I2C-pathia
  s_raw = analogRead(PIN_HALL_ANALOG); // 0..1023

  // I2C-luku lisätään myöhemmin:
  // if (s_i2c_present) {
  //   // TODO: read and map 0..1023
  //   // s_raw = mapped_value;
  // }
}
``