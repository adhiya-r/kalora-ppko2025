#include "loracomms.h"
#include "sensors.h"
#include "sleep.h"
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

// Global variables (definisi)
bool dataRequested = false;
unsigned long lastActivityTime = 0;

bool initializeLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    return false;
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setTxPower(20);

  // Set up DIO0 as an interrupt pin
  pinMode(LORA_DI0, INPUT_PULLUP);
  
  LoRa.onReceive(onLoRaReceive);
  LoRa.receive();

  Serial.println("Kalora Sender Ready - ID: " + String(SENDER_ID));
  return true;
}

void onLoRaReceive(int packetSize) {
  if (packetSize == 0) return;

  String received = "";
  for (int i = 0; i < packetSize; i++) {
    received += (char)LoRa.read();
  }

  Serial.println("Received LoRa: " + received);

  String requestPrefix = "REQ:" + String(SENDER_ID);
  if (received.startsWith(requestPrefix)) {
    Serial.println("Data request received for our ID");
    dataRequested = true;
    lastActivityTime = millis();
  }

  LoRa.receive();
}

void handleDataRequest() {
  if (dataRequested) {
    Serial.println("Processing data request...");
    powerUpSensors();
    delay(500);

    SensorReadings readings = readAllSensors();

    if (readings.valid) {
      String payload = String(SENDER_ID) + "," +
                      String(readings.temperature, 1) + "," +
                      String(readings.lightIntensity, 0) + "," +
                      String(readings.soilMoisture, 1);
      bool sendSuccess = sendLoRaData(payload);
      if (!sendSuccess) {
        Serial.println("Failed to send data after all retries");
      }
    } else {
      Serial.println("Failed to read sensors");
    }

    powerDownSensors();
    dataRequested = false;
    Serial.println("Data request handled. Entering deep sleep.");
  }
}

bool sendLoRaData(String payload) {
  bool sendSuccess = false;
  for (int retry = 0; retry < MAX_RETRY_COUNT && !sendSuccess; retry++) {
    Serial.println("Sending data (attempt " + String(retry + 1) + "): " + payload);
    LoRa.beginPacket();
    LoRa.print(payload);
    int result = LoRa.endPacket();
    if (result == 1) {
      sendSuccess = true;
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Send failed, retrying...");
      delay(1000);
    }
  }
  return sendSuccess;
}