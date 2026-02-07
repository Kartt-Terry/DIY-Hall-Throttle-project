/*
  DIYThrottle_RawCalib - Signals + IIR Filter (Step 2)
  Board: Arduino Pro Micro (select "Arduino Leonardo" in Arduino IDE)
  Baud : 115200

  Features in this version:
    - ADC read (raw1/raw2)
    - Diagnostic voltages: V1 = raw1/1023*Vsys, V2 = raw2/1023*Vsys (Vsys≈5.00V)
    - IIR filtering per channel: f = f + alpha*(raw - f), alpha=0.2
    - CSV output for Serial Plotter: S1,S2,V1,V2,F1,F2
    - Plotter mode toggle 't' (CSV only ON/OFF)
    - Serial calibration kept: x (MAX), n (MIN), s (save), p (print), r (reset)

  Usage notes:
  - Keep Serial Monitor CLOSED when using Serial Plotter.
  - Serial Monitor line ending = Newline or Both NL&CR.
*/

#include <EEPROM.h>

// ---------------- Pins (A1324 outputs) ----------------
const uint8_t PIN_S1 = A0;   // Sensor 1
const uint8_t PIN_S2 = A1;   // Sensor 2

// ---------------- Serial/ADC ----------------
const unsigned long SERIAL_BAUD = 115200;
const float VREF_VOLTS = 5.00f; // diagnostic voltage (later: measure Vcc)

// ---------------- IIR filtering ----------------
float alpha = 0.20f;         // smoothing factor
float f1 = 0.0f, f2 = 0.0f;  // filtered states (LSB units)

// ---------------- EEPROM layout (demo) ----------------
struct Config {
  uint16_t magic; // 0xCAFE = valid
  int16_t  min1;
  int16_t  max1;
  int16_t  min2;
  int16_t  max2;
  uint16_t crc;   // tiny checksum (sum of fields)
};

const uint16_t MAGIC       = 0xCAFE;
const int      EEPROM_ADDR = 0;
Config cfg;

// ---------------- Plotter mode control ----------------
bool plotterMode       = true;   // true = CSV-only output (for Serial Plotter)
bool csvHeaderPrinted  = false;  // print CSV header once after toggling ON

// ---------------- Print rate ----------------
unsigned long lastPrintMs = 0;
const unsigned long PRINT_INTERVAL_MS = 20; // ~50 Hz

// ---------------- Helpers ----------------
uint16_t checksum(const Config& c) {
  uint32_t s = 0;
  s += c.magic;
  s += (uint16_t)c.min1; s += (uint16_t)c.max1;
  s += (uint16_t)c.min2; s += (uint16_t)c.max2;
  return (uint16_t)(s & 0xFFFF);
}

void loadConfig() {
  EEPROM.get(EEPROM_ADDR, cfg);
  if (cfg.magic != MAGIC || cfg.crc != checksum(cfg)) {
    cfg.magic = 0;
    cfg.min1 = 1023; cfg.max1 = 0;
    cfg.min2 = 1023; cfg.max2 = 0;
    cfg.crc  = 0;
    EEPROM.put(EEPROM_ADDR, cfg); // store default invalid
  }
}

void saveConfig() {
  cfg.magic = MAGIC;
  cfg.crc   = checksum(cfg);
  EEPROM.put(EEPROM_ADDR, cfg);
}

void resetConfig() {
  cfg.magic = 0;
  cfg.min1 = 1023; cfg.max1 = 0;
  cfg.min2 = 1023; cfg.max2 = 0;
  cfg.crc  = 0;
  EEPROM.put(EEPROM_ADDR, cfg);
}

void printConfig() {
  if (plotterMode) return; // no chatter in Plotter mode
  Serial.print(F("MIN1=")); Serial.print(cfg.min1);
  Serial.print(F("  MAX1=")); Serial.print(cfg.max1);
  Serial.print(F("  MIN2=")); Serial.print(cfg.min2);
  Serial.print(F("  MAX2=")); Serial.println(cfg.max2);
}

void printHelp() {
  if (plotterMode) return;
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("  ?  -> help"));
  Serial.println(F("  x  -> capture MAX (hold fully down, then press 'x')"));
  Serial.println(F("  n  -> capture MIN (release fully, then press 'n')"));
  Serial.println(F("  s  -> save to EEPROM"));
  Serial.println(F("  p  -> print MIN/MAX"));
  Serial.println(F("  r  -> reset config"));
  Serial.println(F("  t  -> toggle plotter mode (CSV-only ON/OFF)"));
  Serial.println();
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) { /* wait for USB CDC */ }

  analogReference(DEFAULT); // AVCC
  pinMode(PIN_S1, INPUT);
  pinMode(PIN_S2, INPUT);

  loadConfig();

  // Initialize filtered states with first readings to prevent jump
  int r1 = analogRead(PIN_S1);
  int r2 = analogRead(PIN_S2);
  f1 = (float)r1;
  f2 = (float)r2;

  if (!plotterMode) {
    Serial.println(F("DIYThrottle_RawCalib - Step 2 (Signals + IIR)"));
    printConfig();
    printHelp();
  }
}

void loop() {
  // --- 1) ADC read (raw) ---
  int raw1 = analogRead(PIN_S1); // 0..1023
  int raw2 = analogRead(PIN_S2); // 0..1023

  // --- 2) IIR filter ---
  f1 = f1 + alpha * ((float)raw1 - f1);
  f2 = f2 + alpha * ((float)raw2 - f2);

  // --- 3) Periodic output ---
  unsigned long now = millis();
  if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = now;

    float v1 = (raw1 / 1023.0f) * VREF_VOLTS; // diagnostics
    float v2 = (raw2 / 1023.0f) * VREF_VOLTS;

    if (plotterMode) {
      if (!csvHeaderPrinted) {
        Serial.println(F("S1,S2,V1,V2,F1,F2"));
        csvHeaderPrinted = true;
      }
      Serial.print(raw1);       Serial.print(',');
      Serial.print(raw2);       Serial.print(',');
      Serial.print(v1, 3);      Serial.print(',');
      Serial.print(v2, 3);      Serial.print(',');
      Serial.print(f1, 3);      Serial.print(',');
      Serial.println(f2, 3);
    } else {
      Serial.print(F("S1:")); Serial.print(raw1);
      Serial.print(F(" S2:")); Serial.print(raw2);
      Serial.print(F(" V1:")); Serial.print(v1, 3);
      Serial.print(F(" V2:")); Serial.print(v2, 3);
      Serial.print(F(" F1:")); Serial.print(f1, 3);
      Serial.print(F(" F2:")); Serial.print(f2, 3);
      Serial.println();
    }
  }

  // --- serial commands ---
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') continue;

    switch (c) {
      case '?':
        printHelp();
        break;

      case 'x': { // capture MAX (hold pedal fully down)
        cfg.max1 = analogRead(PIN_S1);
        cfg.max2 = analogRead(PIN_S2);
        if (!plotterMode) {
          Serial.print(F("Captured MAX1=")); Serial.print(cfg.max1);
          Serial.print(F(" MAX2=")); Serial.println(cfg.max2);
        }
        break;
      }

      case 'n': { // capture MIN (release pedal fully)
        cfg.min1 = analogRead(PIN_S1);
        cfg.min2 = analogRead(PIN_S2);
        if (!plotterMode) {
          Serial.print(F("Captured MIN1=")); Serial.print(cfg.min1);
          Serial.print(F(" MIN2=")); Serial.println(cfg.min2);
        }
        break;
      }

      case 's': { // save to EEPROM (basic validity check)
        bool ok = (cfg.max1 > cfg.min1 + 5) && (cfg.max2 > cfg.min2 + 5);
        if (!ok) {
          if (!plotterMode) {
            Serial.println(F("ERROR: invalid calibration (do 'x' then 'n' before 's')."));
          }
        } else {
          saveConfig();
          if (!plotterMode) {
            Serial.println(F("Saved calibration."));
            printConfig();
          }
        }
        break;
      }

      case 'p':
        printConfig();
        break;

      case 'r':
        resetConfig();
        if (!plotterMode) {
          Serial.println(F("Config reset. Recalibrate (x,n) then save (s)."));
        }
        break;

      case 't': // toggle Plotter mode
        plotterMode = !plotterMode;
        csvHeaderPrinted = false; // header again if ON
        if (!plotterMode) {
          Serial.println(F("Plotter mode OFF (verbose text enabled)."));
          printConfig();
          printHelp();
        }
        break;

      default:
        if (!plotterMode) {
          Serial.print(F("Unknown command: "));
          Serial.println(c);
          printHelp();
        }
        break;
    }
  }
}
