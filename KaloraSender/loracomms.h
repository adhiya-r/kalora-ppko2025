#ifndef LORACOMMS_H
#define LORACOMMS_H

#include "config.h"

// Global variables for LoRa communication
extern bool dataRequested;
extern unsigned long lastActivityTime;

// Function declarations
bool initializeLoRa();
void onLoRaReceive(int packetSize);
void handleDataRequest();
bool sendLoRaData(String payload);

#endif