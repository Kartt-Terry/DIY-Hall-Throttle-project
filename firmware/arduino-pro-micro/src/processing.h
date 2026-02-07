#pragma once
#include <Arduino.h>

void  processing_init();
void  processing_step();
float processing_get_output();

// Calibration control
void processing_capture_min();
void processing_capture_max();
void processing_set_deadzones(float low, float high);
void processing_set_profile(uint8_t profile, float expo);