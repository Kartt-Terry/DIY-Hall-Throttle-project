#pragma once
#include <Arduino.h>

void storage_init();
void storage_load(int &minRaw, int &maxRaw, float &dzL, float &dzH, uint8_t &profile, float &expo);
void storage_save(int minRaw, int maxRaw, float dzL, float dzH, uint8_t profile, float expo);
void storage_set_min(int v);
void storage_set_max(int v);
