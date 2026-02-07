/*
  DIYThrottle_RawCalib - Step 4 (LUT + Mismatch)
  Board: Arduino Pro Micro (select "Arduino Leonardo" in Arduino IDE)
  Baud : 115200

  Additions in Step 4:
    - LUT profile (65 points, 0..10000 -> 0..1, linear interpolation)
    - Mismatch monitoring: delta = |y1 - y2|, threshold 0.03, hold 100 ms
    - CSV columns: S1,S2,V1,V2,F1,F2,N1,N2,D1,D2,Y1,Y2,DELTA,FAULT,OUT
    - Robust header printing: on toggle, every 2s in plotterMode, and on 'h'

  Serial commands (line ending = Newline or Both NL&CR):
    ?  help (when plotterMode OFF)
    x  capture MAX (hold fully down)
    n  capture MIN (release fully)
    s  save to EEPROM (MIN/MAX, deadzones, profile, expo, LUT)
    p  print MIN/MAX (verbose)
    r  reset config (clears EEPROM header, clears fault)
    t  toggle plotter mode (CSV-only ON/OFF)
    h  print CSV header now
    I  load identity LUT (0..1), not saved until 's'
*/

#include <EEPROM.h>

// ---------------- Pins ----------------
const uint8_t PIN_S1 = A0;   // Sensor 1 (A1324)
const uint8_t PIN_S2 = A1;   // Sensor 2 (A1324)

// ---------------- Serial/ADC ----------------
const unsigned long SERIAL_BAUD = 115200;
const float VREF_VOLTS = 5.00f; // diagnostic supply (later: measured Vcc)

// ---------------- IIR filtering ----------------
float alpha = 0.20f;         // smoothing factor
float f1 = 0.0f, f2 = 0.0f;  // filtered states (LSB)

// ---------------- Profile / LUT ----------------
// profile: 0=linear,1=expo,2=log,3=lut (we use LUT by default)

// ---------------- EEPROM config (v2) ----------------
struct Config {
  uint16_t magic;       // 0xCAFE
  uint8_t  version;     // 2
  uint8_t  _pad;        // align
  int16_t  min1;  int16_t  max1;
  int16_t  min2;  int16_t  max2;
  uint16_t deadLow_mil;   // 0..10000 (milli fraction)
  uint16_t deadHigh_mil;  // 0..10000
  uint8_t  profile;       // see above
  uint8_t  _pad2[3];
  float    expo_k;        // reserved for future expo
  uint16_t lut[65];       // 0..10000
  uint16_t crc;           // simple checksum
};

const uint16_t MAGIC       = 0xCAFE;
const uint8_t  VERSION     = 2;
const int      EEPROM_ADDR = 0;
Config cfg;

// ---------------- Deadzones (runtime) ----------------
float dead_low  = 0.02f;   // 2%
float dead_high = 0.02f;   // 2%

// ---------------- LUT runtime ----------------
uint16_t lutU[65];   // uint16 0..10000 in RAM (mirrors cfg.lut)

// ---------------- Plotter / header ----------------
bool plotterMode       = true;   // CSV-only when true
bool csvHeaderPrinted  = false;
unsigned long lastPrintMs  = 0;
const unsigned long PRINT_INTERVAL_MS = 20;   // ~50 Hz
unsigned long lastHeaderMs = 0;
const unsigned long HEADER_INTERVAL_MS = 2000; // 2 s

// ---------------- Mismatch monitoring ----------------
const float MISMATCH_THRESHOLD = 0.03f;       // 3%
const unsigned long MISMATCH_HOLD_MS = 100;   // must persist this long to trip
const unsigned long MISMATCH_CLEAR_MS = 300;  // below threshold this long to clear
bool faultMismatch = false;
unsigned long mismatchStartMs = 0;
unsigned long mismatchClearStartMs = 0;

// ---------------- Helpers ----------------
static inline uint16_t add16(uint16_t a, uint16_t b){ return (uint16_t)((a + b) & 0xFFFF); }

uint16_t checksum(const Config& c){
  uint16_t s = 0;
  s = add16(s, c.magic);
  s = add16(s, ((uint16_t)c.version));
  s = add16(s, (uint16_t)c.min1); s = add16(s, (uint16_t)c.max1);
  s = add16(s, (uint16_t)c.min2); s = add16(s, (uint16_t)c.max2);
  s = add16(s, c.deadLow_mil);    s = add16(s, c.deadHigh_mil);
  s = add16(s, (uint16_t)c.profile);
  // fold float expo_k into two 16-bit words
  union { float f; uint16_t w[2]; } u; u.f = c.expo_k; s = add16(s, u.w[0]); s = add16(s, u.w[1]);
  for (int i=0;i<65;i++) s = add16(s, c.lut[i]);
  return s;
}

void setIdentityLUT(uint16_t *dst){
  for (int i=0;i<65;i++) dst[i] = (uint16_t)((i * 10000UL) / 64UL); // 0..10000
}

void syncRuntimeFromCfg(){
  dead_low  = cfg.deadLow_mil  / 10000.0f;
  dead_high = cfg.deadHigh_mil / 10000.0f;
  for (int i=0;i<65;i++) lutU[i] = cfg.lut[i];
}

void loadConfig(){
  EEPROM.get(EEPROM_ADDR, cfg);
  bool ok = (cfg.magic == MAGIC && cfg.version == VERSION);
  if (ok){
    uint16_t expect = checksum(cfg);
    ok = (expect == cfg.crc);
  }
  if (!ok){
    cfg.magic = MAGIC;
    cfg.version = VERSION;
    cfg.min1=1023; cfg.max1=0; cfg.min2=1023; cfg.max2=0;
    cfg.deadLow_mil = 200;   // 2%
    cfg.deadHigh_mil= 200;   // 2%
    cfg.profile = 3;         // LUT by default
    cfg.expo_k = 1.6f;
    setIdentityLUT(cfg.lut);
    cfg.crc = checksum(cfg);
    EEPROM.put(EEPROM_ADDR, cfg);
  }
  syncRuntimeFromCfg();
}

void saveConfig(){
  cfg.deadLow_mil  = (uint16_t)(dead_low  * 10000.0f + 0.5f);
  cfg.deadHigh_mil = (uint16_t)(dead_high * 10000.0f + 0.5f);
  for (int i=0;i<65;i++) cfg.lut[i] = lutU[i];
  cfg.crc = checksum(cfg);
  EEPROM.put(EEPROM_ADDR, cfg);
}

void resetConfig(){
  cfg.magic = 0; // mark invalid
  EEPROM.put(EEPROM_ADDR, cfg);
  faultMismatch = false; mismatchStartMs=0; mismatchClearStartMs=0;
}

void printConfig(){
  if (plotterMode) return;
  Serial.print(F("MIN1=")); Serial.print(cfg.min1);
  Serial.print(F("  MAX1=")); Serial.print(cfg.max1);
  Serial.print(F("  MIN2=")); Serial.print(cfg.min2);
  Serial.print(F("  MAX2=")); Serial.print(cfg.max2);
  Serial.print(F("  dead_low="));  Serial.print(dead_low,3);
  Serial.print(F("  dead_high=")); Serial.println(dead_high,3);
}

void printHelp(){
  if (plotterMode) return;
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("  ?  -> help"));
  Serial.println(F("  x  -> capture MAX (hold fully down, then press 'x')"));
  Serial.println(F("  n  -> capture MIN (release fully, then press 'n')"));
  Serial.println(F("  s  -> save to EEPROM"));
  Serial.println(F("  p  -> print MIN/MAX & deadzones"));
  Serial.println(F("  r  -> reset config (clear header) & clear fault"));
  Serial.println(F("  t  -> toggle plotter mode (CSV-only ON/OFF)"));
  Serial.println(F("  h  -> print CSV header now"));
  Serial.println(F("  I  -> load identity LUT (0..1) (not saved until 's')"));
  Serial.println();
}

void printCsvHeader(){
  Serial.println(F("S1,S2,V1,V2,F1,F2,N1,N2,D1,D2,Y1,Y2,DELTA,FAULT,OUT"));
  csvHeaderPrinted = true;
  lastHeaderMs = millis();
}

// map filtered ADC (f) to [0..1]
float normalize_from_adc(float f, int16_t mn, int16_t mx){
  int16_t span = mx - mn; if (span < 1) span = 1;
  float n = (f - (float)mn) / (float)span;
  if (n < 0.0f) n = 0.0f; if (n > 1.0f) n = 1.0f; return n;
}

// apply deadzones in [0..1]
float applyDeadzones(float n){
  if (n < dead_low) return 0.0f;
  if (n > 1.0f - dead_high) return 1.0f;
  return n;
}

// LUT eval: linear interpolation over 65 points (0..64)
float lut_eval(float n){
  if (n <= 0.0f) return lutU[0] / 10000.0f;
  if (n >= 1.0f) return lutU[64] / 10000.0f;
  float idx = n * 64.0f;
  int i = (int)idx; if (i < 0) i = 0; if (i > 63) i = 63;
  float frac = idx - i;
  float a = lutU[i]   / 10000.0f;
  float b = lutU[i+1] / 10000.0f;
  return (1.0f - frac) * a + frac * b;
}

void setup(){
  Serial.begin(SERIAL_BAUD);
  while (!Serial) { /* wait for USB CDC */ }

  analogReference(DEFAULT); // AVCC
  pinMode(PIN_S1, INPUT);
  pinMode(PIN_S2, INPUT);

  loadConfig();

  // init filtered states to first readings
  int r1 = analogRead(PIN_S1);
  int r2 = analogRead(PIN_S2);
  f1 = (float)r1; f2 = (float)r2;

  if (!plotterMode){
    Serial.println(F("DIYThrottle_RawCalib - Step 4 (LUT + Mismatch)"));
    printConfig();
    printHelp();
  } else {
    csvHeaderPrinted = false; lastHeaderMs = 0;
  }
}

void loop(){
  // --- RAW ---
  int raw1 = analogRead(PIN_S1);
  int raw2 = analogRead(PIN_S2);

  // --- IIR ---
  f1 = f1 + alpha * ((float)raw1 - f1);
  f2 = f2 + alpha * ((float)raw2 - f2);

  // --- Normalize & deadzones ---
  float n1 = normalize_from_adc(f1, cfg.min1, cfg.max1);
  float n2 = normalize_from_adc(f2, cfg.min2, cfg.max2);
  float d1 = applyDeadzones(n1);
  float d2 = applyDeadzones(n2);

  // --- LUT profile ---
  float y1 = lut_eval(d1);
  float y2 = lut_eval(d2);

  // --- Mismatch monitoring ---
  float delta = (y1 > y2) ? (y1 - y2) : (y2 - y1);
  unsigned long now = millis();
  if (!faultMismatch){
    if (delta > MISMATCH_THRESHOLD){
      if (mismatchStartMs == 0) mismatchStartMs = now;
      if (now - mismatchStartMs >= MISMATCH_HOLD_MS){
        faultMismatch = true;
        mismatchClearStartMs = 0; // reset clear timer
      }
    } else {
      mismatchStartMs = 0; // not persistent
    }
  } else {
    // try clear when delta low enough for a while
    if (delta <= MISMATCH_THRESHOLD){
      if (mismatchClearStartMs == 0) mismatchClearStartMs = now;
      if (now - mismatchClearStartMs >= MISMATCH_CLEAR_MS){
        faultMismatch = false; // auto clear
        mismatchStartMs = 0; mismatchClearStartMs = 0;
      }
    } else {
      mismatchClearStartMs = 0; // still high
    }
  }

  // --- OUT ---
  float out = 0.5f * (y1 + y2);
  if (faultMismatch){
    out = 0.0f; // fail-safe
  }

  // --- periodic header ---
  if (plotterMode){
    if (!csvHeaderPrinted || (now - lastHeaderMs >= HEADER_INTERVAL_MS)){
      printCsvHeader();
    }
  }

  // --- periodic data print ---
  if (now - lastPrintMs >= PRINT_INTERVAL_MS){
    lastPrintMs = now;
    float v1 = (raw1 / 1023.0f) * VREF_VOLTS;
    float v2 = (raw2 / 1023.0f) * VREF_VOLTS;

    if (plotterMode){
      Serial.print(raw1);  Serial.print(',');
      Serial.print(raw2);  Serial.print(',');
      Serial.print(v1,3);  Serial.print(',');
      Serial.print(v2,3);  Serial.print(',');
      Serial.print(f1,3);  Serial.print(',');
      Serial.print(f2,3);  Serial.print(',');
      Serial.print(n1,3);  Serial.print(',');
      Serial.print(n2,3);  Serial.print(',');
      Serial.print(d1,3);  Serial.print(',');
      Serial.print(d2,3);  Serial.print(',');
      Serial.print(y1,3);  Serial.print(',');
      Serial.print(y2,3);  Serial.print(',');
      Serial.print(delta,3);Serial.print(',');
      Serial.print(faultMismatch ? 1 : 0); Serial.print(',');
      Serial.println(out,3);
    } else {
      Serial.print(F("S1:")); Serial.print(raw1);
      Serial.print(F(" S2:")); Serial.print(raw2);
      Serial.print(F(" V1:")); Serial.print(v1,3);
      Serial.print(F(" V2:")); Serial.print(v2,3);
      Serial.print(F(" F1:")); Serial.print(f1,3);
      Serial.print(F(" F2:")); Serial.print(f2,3);
      Serial.print(F(" N1:")); Serial.print(n1,3);
      Serial.print(F(" N2:")); Serial.print(n2,3);
      Serial.print(F(" D1:")); Serial.print(d1,3);
      Serial.print(F(" D2:")); Serial.print(d2,3);
      Serial.print(F(" Y1:")); Serial.print(y1,3);
      Serial.print(F(" Y2:")); Serial.print(y2,3);
      Serial.print(F(" DELTA:")); Serial.print(delta,3);
      Serial.print(F(" FAULT:")); Serial.print(faultMismatch ? 1 : 0);
      Serial.print(F(" OUT:")); Serial.print(out,3);
      Serial.println();
    }
  }

  // --- serial commands ---
  while (Serial.available()){
    char c = Serial.read();
    if (c=='\r' || c=='\n') continue;
    switch (c){
      case '?': printHelp(); break;
      case 'x': {
        cfg.max1 = analogRead(PIN_S1); cfg.max2 = analogRead(PIN_S2);
        if (!plotterMode){ Serial.print(F("Captured MAX1=")); Serial.print(cfg.max1); Serial.print(F(" MAX2=")); Serial.println(cfg.max2);} break; }
      case 'n': {
        cfg.min1 = analogRead(PIN_S1); cfg.min2 = analogRead(PIN_S2);
        if (!plotterMode){ Serial.print(F("Captured MIN1=")); Serial.print(cfg.min1); Serial.print(F(" MIN2=")); Serial.println(cfg.min2);} break; }
      case 's': {
        bool ok = (cfg.max1 > cfg.min1 + 5) && (cfg.max2 > cfg.min2 + 5);
        if (!ok){ if (!plotterMode) Serial.println(F("ERROR: invalid calibration (do 'x' then 'n' before 's').")); }
        else { saveConfig(); if (!plotterMode){ Serial.println(F("Saved config.")); printConfig(); } }
        break; }
      case 'p': printConfig(); break;
      case 'r': resetConfig(); if (!plotterMode) Serial.println(F("Config reset & fault cleared.")); loadConfig(); break;
      case 't': plotterMode = !plotterMode; csvHeaderPrinted=false; lastHeaderMs=0; if (!plotterMode){ Serial.println(F("Plotter mode OFF (verbose text enabled).")); printConfig(); printHelp(); } break;
      case 'h': if (plotterMode) printCsvHeader(); break;
      case 'I': setIdentityLUT(lutU); if (!plotterMode) Serial.println(F("Identity LUT loaded (not saved).")); break;
      default:
        if (!plotterMode){ Serial.print(F("Unknown cmd: ")); Serial.println(c); printHelp(); }
        break;
    }
  }
}
