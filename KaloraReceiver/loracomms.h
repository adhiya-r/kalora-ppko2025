#ifndef LORACOMMS_H
#define LORACOMMS_H

#include "config.h" // Untuk DATA_REQUEST_TIMEOUT, structs, dll.

// Function declarations
bool initializeLoRa();
void handleLoRaReceive(int packetSize); // <<< UBAH SIGNATURE: TAMBAHKAN PARAMETER
void requestDataFromSender(String targetID); // <<< UBAH SIGNATURE: TAMBAHKAN PARAMETER
void parseSensorData(String data);
void processReceivedPayload();

// Global variables untuk state request data (TAMBAHKAN INI)
extern bool isRequestingData;
extern unsigned long requestStartTime;
extern String currentRequestedSenderID;
extern int currentDots; // Untuk animasi "Waiting..."
extern String receivedPayloadBuffer;
extern bool isPayloadReady;

// Fungsi baru yang akan dipanggil di loop() untuk manajemen request non-blocking (TAMBAHKAN INI)
void handleDataRequestState();

#endif