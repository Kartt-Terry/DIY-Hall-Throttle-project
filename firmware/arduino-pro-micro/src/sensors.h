#pragma once
#include <Arduino.h>

void sensors_init();
void sensors_poll();
int  sensors_get_raw();
bool sensors_detect_i2c();
bool sensors_ok();