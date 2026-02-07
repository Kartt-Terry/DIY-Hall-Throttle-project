#pragma once
#include <Arduino.h>
#include "types.h"

class StateMachine {
public:
  void init();
  void update();
  Mode mode() const { return m_mode; }

  // Julkinen setter, jota globaali set_mode() k‰ytt‰‰
  void setMode(Mode m) { m_mode = m; }

private:
  void check_faults();
  void check_boot_gesture();

  Mode m_mode = MODE_DRIVING;
  unsigned long m_bootStart = 0;
  uint8_t m_pressCount = 0;
  bool m_wasPressed = false;
};

// Globaali apufunktio protokollalle
void set_mode(Mode m);