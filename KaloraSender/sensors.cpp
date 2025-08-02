#include "sensors.h"
#include <Arduino.h>
#include <math.h>

void initializeSensors() {
  // Initialize sensor power control
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH); // Turn on sensors
}

void powerUpSensors() {
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  delay(500); // Allow sensors to stabilize
}

void powerDownSensors() {
  digitalWrite(SENSOR_POWER_PIN, LOW);
}

SensorReadings readAllSensors() {
  SensorReadings readings;
  readings.valid = false;
  
  Serial.println("Reading sensors...");
  
  // Read temperature from NTC sensor
  readings.temperature = readNTCTemperature();
  Serial.println("Temperature: " + String(readings.temperature, 1) + "°C");
  
  // Read light intensity from LDR
  readings.lightIntensity = readLDRLightIntensity();
  Serial.println("Light intensity: " + String(readings.lightIntensity, 0) + " lux");
  
  // Read soil moisture from capacitive sensor
  readings.soilMoisture = readSoilMoisture();
  Serial.println("Soil moisture: " + String(readings.soilMoisture, 1) + "%");
  
  // Validate readings (basic sanity check)
  if (readings.temperature > -40 && readings.temperature < 80 &&
      readings.lightIntensity >= 0 && readings.lightIntensity <= 50000 &&
      readings.soilMoisture >= 0 && readings.soilMoisture <= 100) {
    readings.valid = true;
  }
  
  return readings;
}

float readNTCTemperature() {
  // Take multiple readings for stability
  int rawSum = 0;
  const int numReadings = 10;
  
  for (int i = 0; i < numReadings; i++) {
    rawSum += analogRead(NTC_PIN);
    delay(10);
  }
  
  int rawValue = rawSum / numReadings;
  
  Serial.print("NTC Raw ADC: ");
  Serial.print(rawValue);

  // Convert ADC reading to resistance
  float resistance = NTC_SERIES_RESISTOR / ((4095.0 / rawValue) - 1.0);
  Serial.print("NTC Resistance: ");
  Serial.print(resistance, 1);

  // Calculate temperature using Steinhart-Hart equation (simplified Beta formula)
  float steinhart = resistance / NTC_NOMINAL_RESISTANCE;     // (R/Ro)
  steinhart = log(steinhart);                                // ln(R/Ro)
  steinhart /= NTC_B_COEFFICIENT;                           // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NTC_NOMINAL_TEMPERATURE + 273.15);   // + (1/To)
  steinhart = 1.0 / steinhart;                             // Invert
  steinhart -= 273.15;                                     // Convert to Celsius
  
  return steinhart;
}

float readLDRLightIntensity() {
  // Take multiple readings for stability
  int rawSum = 0;
  const int numReadings = 10;
  
  for (int i = 0; i < numReadings; i++) {
    rawSum += analogRead(LDR_PIN);
    delay(10);
  }
  
  int rawValue = rawSum / numReadings;
  
  Serial.print("LDR Raw ADC: ");
  Serial.print(rawValue);

  // Convert ADC reading to approximate lux value
  // This is a simplified conversion - you may need to calibrate for your specific LDR
  float voltage = (rawValue / 4095.0) * 3.3;
  float resistance = (3.3 - voltage) / voltage * 10000; // Assuming 10K pull-up
  
  // Simple LDR to lux conversion (approximation for GL5516)
  // This formula may need adjustment based on actual calibration
  float lux = 12518931 * pow(resistance, -1.405);
  
  // Clamp to reasonable range
  if (lux > 50000) lux = 50000;
  if (lux < 0) lux = 0;
  
  return lux;
}

float readSoilMoisture() {
  // Take multiple readings for stability
  int rawSum = 0;
  const int numReadings = 10;
  
  for (int i = 0; i < numReadings; i++) {
    rawSum += analogRead(SOIL_SENSOR_PIN);
    delay(10);
  }
  
  int rawValue = rawSum / numReadings;

  Serial.print("Soil Moisture Raw ADC: ");
  Serial.print(rawValue);
  
  // Convert raw ADC reading to moisture percentage
  // Capacitive soil sensor typically gives:
  // - Dry soil: ~3000-4095 (lower moisture)
  // - Wet soil: ~1300-2000 (higher moisture)
  // These values may need calibration for your specific sensor
  
  const int dryValue = 3000;   // ADC reading in dry soil
  const int wetValue = 1500;   // ADC reading in wet soil
  
  // Convert to percentage (inverted because lower ADC = more moisture)
  float moisturePercent = map(rawValue, dryValue, wetValue, 0, 100);
  
  // Clamp to 0-100% range
  if (moisturePercent > 100) moisturePercent = 100;
  if (moisturePercent < 0) moisturePercent = 0;
  
  return rawValue;
}