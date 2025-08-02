#include "loracomms.h"
#include "display.h" // Perlu untuk showRequestingData, showMainMenu, displayError
#include "data.h"    // Perlu untuk getSendersArray, getLatestData
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

// Global variables (DEFINISI VARIABEL GLOBAL INI)
bool dataReceived = false;
bool isRequestingData = false;
unsigned long requestStartTime = 0;
String currentRequestedSenderID = "";
int currentDots = 0;

// Variabel global baru untuk menyimpan payload yang diterima dari interrupt
String receivedPayloadBuffer = "";
bool isPayloadReady = false;

bool initializeLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    return false;
  }
  
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setTxPower(20);
  
  LoRa.onReceive(handleLoRaReceive); // <<< SET CALLBACK UNTUK PENERIMAAN
  LoRa.receive(); // Masuk mode terima awal
  
  Serial.println("Kalora Receiver Ready");
  return true;
}

// Handler LoRa Receive (dipanggil otomatis oleh library LoRa)
void handleLoRaReceive(int packetSize) {
  if (packetSize == 0) return;

  // Baca paket dan simpan ke buffer global
  receivedPayloadBuffer = "";
  while (LoRa.available()) {
    receivedPayloadBuffer += (char)LoRa.read();
  }

  // Set flag bahwa payload sudah siap untuk diproses
  isPayloadReady = true;

  // Lanjutkan mendengarkan untuk paket berikutnya
  LoRa.receive();
}

void processReceivedPayload() {
  if (!isPayloadReady) {
    return; // Keluar jika belum ada payload yang siap
  }

  // Reset flag segera
  isPayloadReady = false;

  Serial.println("Received LoRa: " + receivedPayloadBuffer);

  // Lakukan semua logika yang lambat di sini, di loop() utama
  if (receivedPayloadBuffer.indexOf(',') > 0) {
    int firstComma = receivedPayloadBuffer.indexOf(',');
    String receivedSenderID = receivedPayloadBuffer.substring(0, firstComma);

    if (isRequestingData && receivedSenderID == currentRequestedSenderID) {
        parseSensorData(receivedPayloadBuffer);
        isRequestingData = false;
        showMainMenu(); // Memanggil showMainMenu() di loop() sekarang aman
    } else if (!isRequestingData) {
        parseSensorData(receivedPayloadBuffer);
    }
  }

  // Clear buffer
  receivedPayloadBuffer = "";
}

// Fungsi ini HANYA untuk MENGIRIM permintaan, tidak ada delay atau loop blocking
void requestDataFromSender(String targetID) { // <<< UBAH SIGNATURE: TAMBAHKAN PARAMETER
  currentRequestedSenderID = targetID;
  isRequestingData = true; // Set flag sedang requesting
  requestStartTime = millis(); // Catat waktu mulai request
  currentDots = 0; // Reset animasi

  // Send request via LoRa
  String request = "REQ:" + targetID;
  LoRa.beginPacket();
  LoRa.print(request);
  LoRa.endPacket();
  
  Serial.println("Data request sent to: " + targetID);
  
  // Set menu ke state REQUEST_DATA dan update display
  currentMenu = MENU_REQUEST_DATA; // Perlu extern MenuState currentMenu; di loracomms.h (atau display.h)
  updateMenuDisplay(); // Tampilkan layar "Requesting Data..."
}

// Fungsi baru yang akan dipanggil berulang kali di loop() utama (TAMBAHKAN FUNGSI INI)
void handleDataRequestState() {
  if (isRequestingData) {
    unsigned long currentTime = millis(); // Ambil waktu saat ini

    // Update animasi titik setiap 500ms
    if (currentTime - requestStartTime > (currentDots + 1) * 500 && currentDots < 3) {
      currentDots++;
      updateMenuDisplay(); // Refresh display dengan titik baru
    } else if (currentTime - requestStartTime > (currentDots + 1) * 500 && currentDots >= 3) {
      currentDots = 0; // Reset titik setelah 3 titik
      updateMenuDisplay();
    }

    // Cek timeout request
    if (currentTime - requestStartTime > DATA_REQUEST_TIMEOUT) { // DATA_REQUEST_TIMEOUT dari config.h
      Serial.println("Request data timeout!");
      isRequestingData = false; // Hentikan status request
      displayError("Request Timeout!"); // Tampilkan pesan error
      // Berikan jeda singkat agar pesan error terlihat, lalu kembali ke menu utama
      delay(2000); // <<< Satu-satunya delay blocking yang boleh di sini (karena ini adalah final state)
      showMainMenu(); // Kembali ke menu utama
    }
  }
}

void parseSensorData(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);
  int thirdComma = data.indexOf(',', secondComma + 1);
  
  if (firstComma > 0 && secondComma > 0 && thirdComma > 0) {
    SensorData* latestData = getLatestData();
    
    latestData->senderID = data.substring(0, firstComma);
    latestData->temperature = data.substring(firstComma + 1, secondComma).toFloat();
    latestData->soilMoisture = data.substring(secondComma + 1, thirdComma).toFloat();
    latestData->lightIntensity = data.substring(thirdComma + 1).toFloat();
    latestData->timestamp = millis();
    latestData->valid = true;
    dataReceived = true; // Set flag dataReceived
    
    Serial.println("Data parsed successfully");
    Serial.println("ID: " + latestData->senderID);
    Serial.println("Temp: " + String(latestData->temperature));
    Serial.println("Soil: " + String(latestData->soilMoisture));
    Serial.println("Light: " + String(latestData->lightIntensity));
  } else {
    Serial.println("Error parsing sensor data: " + data);
  }
}