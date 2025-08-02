/*
 * Kalora (Karacak Long Range) - Sender
 * Sistem Pemantauan Hutan Pangan
 * 
 * Hardware:
 * - ESP32 DevKit v1
 * - RFM95W LoRa Module
 * - NTC 3950 10K 1M (Temperature)
 * - LDR GL5516 (Light Intensity)
 * - Capacitive Soil Sensor V2.0
 * - Helical Antenna
 * - Power Bank
*/

#include "config.h"
#include "sensors.h"
#include "loracomms.h"
#include "sleep.h"
#include <Arduino.h>

extern bool dataRequested;
extern unsigned long lastActivityTime;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.println("--- KALORA SENDER STARTING ---");
  
  // Increment boot number and get it from sleep.cpp
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  
  // Print wakeup reason
  print_wakeup_reason();
  
  // Initialize LoRa
  if (!initializeLoRa()) {
    Serial.println("LoRa initialization failed! Entering deep sleep...");
    enterDeepSleep();
  }
  
  // Initialize sensors (power control only)
  initializeSensors();
  powerDownSensors(); // Power down sensors to save energy
  
  Serial.println("Waiting for data request...");
  
  lastActivityTime = millis();
}

void loop() {
  // Check for timeout - go to sleep if no activity
  if (millis() - lastActivityTime > ACTIVITY_TIMEOUT) {
    Serial.println("No activity timeout - entering deep sleep");
    enterDeepSleep();
  }
  
  // If data was requested, read sensors and send data
  if (dataRequested) {
    handleDataRequest();
    dataRequested = false;
    
    // After sending data, sleep
    // We will not use delay() here to avoid blocking, just go to sleep immediately
    enterDeepSleep();
  }
  
  // A small delay to prevent watchdog issues
  delay(10); 
}
