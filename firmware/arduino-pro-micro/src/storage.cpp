#include "storage.h"
#include <EEPROM.h>

struct Config {
  uint16_t magic = 0xCAFE;
  int16_t minRaw = 200;
  int16_t maxRaw = 900;
  float dzL = 0.0f;
  float dzH = 0.0f;
  uint8_t profile = 0; // 0=linear
  float expo = 1.6f;
};

static Config cfg;

void storage_init() {
  Config tmp; EEPROM.get(0, tmp);
  if (tmp.magic == 0xCAFE) cfg = tmp; else EEPROM.put(0, cfg);
}

void storage_load(int &minRaw, int &maxRaw, float &dzL, float &dzH, uint8_t &profile, float &expo) {
  minRaw = cfg.minRaw; maxRaw = cfg.maxRaw; dzL = cfg.dzL; dzH = cfg.dzH; profile = cfg.profile; expo = cfg.expo;
}

void storage_save(int minRaw, int maxRaw, float dzL, float dzH, uint8_t profile, float expo) {
  cfg.minRaw = minRaw; cfg.maxRaw = maxRaw; cfg.dzL = dzL; cfg.dzH = dzH; cfg.profile = profile; cfg.expo = expo;
  EEPROM.put(0, cfg);
}

void storage_set_min(int v) { cfg.minRaw = v; EEPROM.put(0, cfg); }
void storage_set_max(int v) { cfg.maxRaw = v; EEPROM.put(0, cfg); }
