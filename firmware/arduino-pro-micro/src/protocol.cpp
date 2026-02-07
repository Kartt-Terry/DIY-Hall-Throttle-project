#include "protocol.h"
#include "processing.h"
#include "state_machine.h"
#include <Arduino.h>

// Erittäin kevyt rivinvast. komentotulkki
static String line;

static void handle_cmd(const String &s) {
  if (s.indexOf("calibrate_min") >= 0) {
    processing_capture_min();
  } else if (s.indexOf("calibrate_max") >= 0) {
    processing_capture_max();
  } else if (s.indexOf("set_mode") >= 0) {
    if (s.indexOf("diagnostics") >= 0)      set_mode(MODE_DIAGNOSTICS);
    else if (s.indexOf("settings") >= 0)     set_mode(MODE_SETTINGS);
    else if (s.indexOf("driving") >= 0)      set_mode(MODE_DRIVING);
    else if (s.indexOf("fault") >= 0)        set_mode(MODE_FAULT);
  } else if (s.indexOf("set_deadzones") >= 0) {
    // TODO: parse numbers (simple protocol later)
  } else if (s.indexOf("set_profile") >= 0) {
    // TODO: parse profile & expo
  } else if (s.indexOf("save") >= 0) {
    // TODO: storage_save(...)
  }
}

void protocol_poll() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (line.length() > 0) {
        handle_cmd(line);
        line = "";
      }
    } else {
      line += c;
    }
  }
}