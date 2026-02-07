/*
  DIYThrottle_RawCalib - Full Sketch with Plotter Mode (CSV) + Serial Calibration
  Board: Arduino Pro Micro (select "Arduino Leonardo" in Arduino IDE)
  Baud : 115200

  Serial commands (set line ending = Newline or Both NL & CR):
    ?  -> help (when Plotter mode is OFF)
    x  -> capture MAX (press pedal fully, then send 'x')
    n  -> capture MIN (release pedal fully, then send 'n')
    s  -> save MIN/MAX to EEPROM
    p  -> print current MIN/MAX
    r  -> reset config (clear EEPROM header)
    t  -> toggle Plotter mode (CSV only ON/OFF)

  Notes:
  - Plotter mode ON prints CSV lines ("S1,S2,V1,V2" header once, then data lines).
  - Plotter mode OFF prints human-readable "label:value" lines & help texts.
  - Keep Serial Monitor CLOSED when using Serial Plotter.
*/

#include <EEPROM.h>

/// --- Pins (A1324 outputs) ---
const uint8_t PIN_S1 = A0;   // Sensor 1
const uint8_t PIN_S2 = A1;   // Sensor 2

/// --- Serial/ADC ---
const unsigned long SERIAL_BAUD = 115200;
const float VREF_VOLTS = 5.00f; // diagnostic voltage (can be replaced by measured Vcc later)

/// --- EEPROM layout (very small demo) ---
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

/// --- Plotter mode control ---
bool plotterMode       = true;  // true = CSV-only output (for Serial Plotter)
bool csvHeaderPrinted  = false; // print header once after toggling ON

/// --- Print rate ---
unsigned long lastPrintMs = 0;
const unsigned long PRINT_INTERVAL_MS = 20; // ~50 Hz

// simple checksum for demo
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
  if (plotterMode) return; // no help spam in Plotter mode
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

  // When Plotter mode is OFF, show banner & help; keep silent otherwise.
  if (!plotterMode) {
    Serial.println(F("DIYThrottle_RawCalib - Pro Micro/Leonardo"));
    printConfig();
    printHelp();
  }
}

void loop() {
  // --- 1) Read raw sensors ---
  int raw1 = analogRead(PIN_S1); // 0..1023
  int raw2 = analogRead(PIN_S2); // 0..1023

  // --- 2) Periodic print (CSV for Plotter, labels for Monitor) ---
  unsigned long now = millis();
  if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = now;

    float v1 = (raw1 / 1023.0f) * VREF_VOLTS;
    float v2 = (raw2 / 1023.0f) * VREF_VOLTS;

    if (plotterMode) {
      // CSV output (best for Serial Plotter)
      if (!csvHeaderPrinted) {
        Serial.println(F("S1,S2,V1,V2")); // header once
        csvHeaderPrinted = true;
      }
      Serial.print(raw1);
      Serial.print(',');
      Serial.print(raw2);
      Serial.print(',');
      Serial.print(v1, 3);
      Serial.print(',');
      Serial.println(v2, 3);
    } else {
      // Human-readable (OK also for Plotter, but CSV is more robust)
      Serial.print(F("S1:")); Serial.print(raw1);
      Serial.print(F(" S2:")); Serial.print(raw2);
      Serial.print(F(" V1:")); Serial.print(v1, 3);
      Serial.print(F(" V2:")); Serial.print(v2, 3);
      Serial.println();
    }
  }

  // --- 3) Serial commands ---
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

      case 's': { // save to EEPROM (with simple validity check)
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
        csvHeaderPrinted = false; // force header next time if we switched ON
        if (!plotterMode) {
          // Now verbose mode: print banner/help/config again
          Serial.println(F("Plotter mode OFF (verbose text enabled)."));
          printConfig();
          printHelp();
        } else {
          // Now CSV-only: keep quiet; next periodic print will output the header and data rows
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