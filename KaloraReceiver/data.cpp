#include "data.h"
#include <Arduino.h>

// Global data storage
static SensorData latestData;

// Registered senders (hardcoded for initial testing)
static RegisteredSender senders[] = {
  {"RW050101", "Sensor Utama Area A", true},
  {"RW050102", "Sensor Cadangan Area B", false}
};

static const int senderCount = sizeof(senders) / sizeof(senders[0]);

void initializeData() {
  latestData.valid = false;
  latestData.senderID = "";
  latestData.temperature = 0.0;
  latestData.soilMoisture = 0.0;
  latestData.lightIntensity = 0.0;
  latestData.timestamp = 0;
}

SensorData* getLatestData() {
  return &latestData;
}

RegisteredSender* getSendersArray() {
  return senders;
}

int getSenderCount() {
  return senderCount;
}