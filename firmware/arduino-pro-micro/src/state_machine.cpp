#include "state_machine.h"
#include "config.h"
#include "sensors.h"
#include "processing.h"
#include <Arduino.h>

static StateMachine* g_self = nullptr;

void StateMachine::init() {
  g_self = this;
  m_mode = MODE_DRIVING;
  m_bootStart = millis();
  m_pressCount = 0;
  m_wasPressed = false;
}

void StateMachine::check_faults() {
  if (!sensors_ok()) {
    m_mode = MODE_FAULT;
  }
}

void StateMachine::check_boot_gesture() {
  if (millis() - m_bootStart > BOOT_GESTURE_WINDOW_MS) return;

  int  raw     = sensors_get_raw();
  bool pressed = (raw >= PRESS_THRESHOLD);

  if (pressed && !m_wasPressed) {
    m_pressCount++;
  }
  m_wasPressed = pressed;

  if (m_pressCount >= REQUIRED_PRESSES) {
    m_mode = MODE_SETTINGS; // enter settings for calibration
    // Täällä voidaan halutessa vain vaihtaa tilaa; varsinainen UI-kalibrointi on tarkempi
  }
}

void StateMachine::update() {
  check_faults();
  check_boot_gesture();
}

void set_mode(Mode m) {
  if (g_self) {
    // Voit halutessa tehdä muuta tilaan siirtymisen yhteydessä
    g_self->setMode(m);
  }
}