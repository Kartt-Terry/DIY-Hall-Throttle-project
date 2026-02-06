#pragma once

// ADC pin for A1324
constexpr uint8_t PIN_HALL_ANALOG = A0;

// I2C address for SparkFun 23880 board (placeholder)
constexpr uint8_t I2C_ADDR_23880 = 0x10; // TODO: set correct address

// Serial
constexpr unsigned long SERIAL_BAUD = 115200;

// Boot gesture timing (ms)
constexpr uint32_t BOOT_GESTURE_WINDOW_MS = 5000;    // time window after boot
constexpr uint16_t PRESS_THRESHOLD = 900;            // raw ADC threshold for "full press" (0–1023)
constexpr uint8_t REQUIRED_PRESSES = 3;              // triple press

// Filtering
constexpr float IIR_ALPHA = 0.2f;
