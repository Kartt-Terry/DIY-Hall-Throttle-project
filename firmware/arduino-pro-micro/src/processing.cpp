#include "processing.h"
#include "config.h"
#include "sensors.h"
#include "storage.h"
#include <Arduino.h>
#include <math.h>   // powf, log10f


static int g_minRaw = 200;     // defaults (placeholder)
static int g_maxRaw = 900;
static float g_filtered = 0.0f;
static float g_out = 0.0f;
static float g_deadLow = 0.00f;
static float g_deadHigh = 0.00f;
static uint8_t g_profile = 0; // 0=linear,1=expo,2=log
static float g_expo = 1.6f;

void processing_init() {
  // Load from EEPROM if available
  storage_load(g_minRaw, g_maxRaw, g_deadLow, g_deadHigh, g_profile, g_expo);
}

static float apply_profile(float x) {
  x = constrain(x, 0.0f, 1.0f);
  switch (g_profile) {
    case 1: // exponential
      return powf(x, g_expo);
    case 2: // logarithmic (simple form)
      return log10f(9.0f * x + 1.0f);
    default:
      return x;
  }
}

void processing_step() {
  int raw = sensors_get_raw();
  // IIR filter
  g_filtered = g_filtered + IIR_ALPHA * ((float)raw - g_filtered);

  // Calibration map
  float norm = (g_filtered - g_minRaw) / (float)(max(1, g_maxRaw - g_minRaw));
  norm = constrain(norm, 0.0f, 1.0f);

  // Deadzones
  if (norm < g_deadLow) norm = 0.0f;
  if (norm > 1.0f - g_deadHigh) norm = 1.0f;

  // Profile
  g_out = apply_profile(norm);
}

float processing_get_output() { return g_out; }

void processing_capture_min() { storage_set_min(g_minRaw = sensors_get_raw()); }
void processing_capture_max() { storage_set_max(g_maxRaw = sensors_get_raw()); }

void processing_set_deadzones(float low, float high) {
  g_deadLow = constrain(low, 0.0f, 0.2f);
  g_deadHigh = constrain(high, 0.0f, 0.2f);
}

void processing_set_profile(uint8_t profile, float expo) {
  g_profile = profile;
  g_expo = expo;
}
