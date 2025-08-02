/*
 * Kalora (Karacak Long Range) - Receiver
 * Sistem Pemantauan Hutan Pangan
 * * Hardware:
 * - ESP32 DevKit v1
 * - RFM95W LoRa Module
 * - OLED 128x64 (SSD1306, I2C)
 * - Push Button Joystick Dual Axis
 * - 12 dBi Antenna
 */

#include "config.h"
#include "display.h"
#include "input.h"
#include "loracomms.h"
#include "data.h"
#include "dashboard.h"

void setup() {
  Serial.begin(115200);
  initializeData();
  
  if (!initializeDisplay()) {
    Serial.println("Display initialization failed!");
    while(1);
  }
  showBootScreen();
  
  initializeJoystick();
  
  if (!initializeLoRa()) { // LoRa.onReceive sudah diatur di sini sekarang
    displayError("LoRa Init Failed!");
    while (1);
  }
  
  initializeDashboard(); // Jika sudah siap
  
  Serial.println("Kalora Receiver Ready");
  
  delay(2000);
  showMainMenu();
}

void loop() {
  handleJoystickInput(); // Mengelola input joystick dan perubahan menu
  
  // Panggil fungsi manajemen state request data non-blocking
  handleDataRequestState();
  
  handleDashboardClient(); // Untuk web server
  processReceivedPayload();
}
