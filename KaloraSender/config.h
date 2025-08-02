#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin definitions for LoRa
#define LORA_SCK     18   // Sesuai pinout SPI standar ESP32
#define LORA_MISO    19   // Sesuai pinout SPI standar ESP32
#define LORA_MOSI    23   // Sesuai pinout SPI standar ESP32
#define LORA_SS      5    // Pin CS (Chip Select) Anda
#define LORA_RST     14   // Pin Reset Anda
#define LORA_DI0     2    // Pin DIO0 Anda (GPIO2 untuk LED onboard)

// Pin definitions for Sensors
#define NTC_PIN         34  // Temperature sensor (NTC 3950)
#define LDR_PIN         35  // Light sensor (LDR GL5516)
#define SOIL_SENSOR_PIN 32  // Capacitive Soil Sensor V2.0
#define SENSOR_POWER_PIN 25 // Power control for sensors

// Sensor configuration
#define NTC_SERIES_RESISTOR 10000  // 10K ohm series resistor
#define NTC_NOMINAL_RESISTANCE 10000  // 10K ohm at 25°C
#define NTC_NOMINAL_TEMPERATURE 25  // Temperature for nominal resistance
#define NTC_B_COEFFICIENT 3380  // Beta coefficient

// System configuration
#define SENDER_ID "RW050101"  // Hardcoded for initial testing
#define SLEEP_TIME_SECONDS 30 // Deep sleep duration
#define MAX_RETRY_COUNT 3
#define ACTIVITY_TIMEOUT 60000 // 1 minute timeout

// Sensor readings structure
struct SensorReadings {
  float temperature;
  float soilMoisture;
  float lightIntensity;
  bool valid;
};

#endif