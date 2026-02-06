#pragma once

enum Mode {
  MODE_DRIVING,
  MODE_SETTINGS,
  MODE_FAULT,
  MODE_DIAGNOSTICS
};

struct Telemetry {
  int raw;
  float filtered;
  float norm;   // 0..1 after calibration
  float out;    // after deadzones + profile
  const char* profile;
  Mode mode;
};
