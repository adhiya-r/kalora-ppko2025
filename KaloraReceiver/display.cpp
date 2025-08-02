#include "display.h"
#include "data.h"
#include "dashboard.h"
#include "loracomms.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <LoRa.h>
#include <Arduino.h>

// Display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Menu variables
MenuState currentMenu = MENU_MAIN;
int menuIndex = 0;
const int maxMenuItems = 6;

bool initializeDisplay() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Alokasi SSD1306 gagal!"));
    return false;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  return true;
}

void showBootScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 10);
  display.println("KALORA");
  display.setTextSize(1);
  display.setCursor(10, 35);
  display.println("Forest Monitoring");
  display.setCursor(30, 45);
  display.println("Receiver v1.0");
  display.setCursor(45, 60);
  display.println("FMSC 2025");
  display.setCursor(60, 75);
  display.println("IG: @adhiya.r");
  display.display();
}

void displayError(String message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ERROR:");
  display.println(message);
  display.display();
}

void showMainMenu() {
  currentMenu = MENU_MAIN;
  menuIndex = 0;
  updateMenuDisplay();
}

void updateMenuDisplay() {
  display.clearDisplay();
  display.setTextSize(1);

  String menuItems[] = {
    "1. Lihat Sensor",
    "2. Minta Data",
    "3. Data Terakhir",
    "4. Pengaturan",
    "5. Akses Web",
    "6. Info Sistem"
  };

  switch (currentMenu) {
    case MENU_MAIN:
      display.setCursor(0, 0);
      display.println("=== MENU KALORA ===");

      for (int i = 0; i < (sizeof(menuItems)/sizeof(menuItems[0])); i++) {
        display.setCursor(0, 12 + i * 8);
        if (i == menuIndex) {
          display.print("> ");
        } else {
          display.print("  ");
        }
        display.println(menuItems[i]);
      }
      break;

    case MENU_SENSORS:
      showSensorsList();
      break;

    case MENU_VIEW_DATA:
      showSensorData();
      break;
    
    case MENU_SETTINGS:
      showSettingsMenu();
      break;

    case MENU_SYSTEM_INFO:
      showSystemInfo();
      break;

    case MENU_DASHBOARD:
      showDashboardInfo(""); // Ubah parameter sesuai kebutuhan, IP mungkin dari global
      break;

    case MENU_REQUEST_DATA: // <<< TAMBAHKAN CASE INI!
      // Memanggil fungsi untuk menampilkan status request
      showRequestingData(currentRequestedSenderID, currentDots); // Variabel ini dari loracomms.h
      break;
    
    default:
        // Handle unexpected menu state or do nothing
        break;
  }

  display.display();
}

void showSensorsList() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== SENSORS ===");
  
  RegisteredSender* senders = getSendersArray();
  int senderCount = getSenderCount();
  
  for (int i = 0; i < senderCount; i++) {
    display.setCursor(0, 12 + i * 16);
    if (i == menuIndex) {
      display.print("> ");
    } else {
      display.print("  ");
    }
    display.println(senders[i].id);
    display.setCursor(4, 20 + i * 16);
    display.println(senders[i].description);
  }
  
  display.setCursor(0, 56);
  display.println("Tekan:Kembali ke Menu");
  display.display();
}

void showSensorData() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== DATA SENSOR ===");
  
  SensorData* latestData = getLatestData();
  
  if (latestData->valid) {
    display.setCursor(0, 12);
    display.print("ID: ");
    display.println(latestData->senderID);
    
    display.setCursor(0, 24);
    display.print("Suhu: ");
    display.print(latestData->temperature, 1);
    display.println(" C");
    
    display.setCursor(0, 32);
    display.print("Lembab: ");
    display.print(latestData->soilMoisture, 1);
    display.println(" %");
    
    display.setCursor(0, 40);
    display.print("Cahaya: ");
    display.print(latestData->lightIntensity, 0);
    display.println(" lux");
    
    // Show timestamp
    unsigned long age = (millis() - latestData->timestamp) / 1000;
    display.setCursor(0, 48);
    display.print("Age: ");
    display.print(age);
    display.println(" sec");
  } else {
    display.setCursor(0, 24);
    display.println("Belum ada data.");
    display.setCursor(0, 32);
    display.println("Minta data dahulu!");
  }
  
  display.setCursor(0, 56);
  display.println("Tekan:Kembali ke Menu");
  display.display();
}

void showSettingsMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== PENGATURAN ===");
  display.setCursor(0, 16);
  display.println("Freq LoRa: 915 MHz");
  display.setCursor(0, 24);
  display.print("ID Penerima: ");
  display.println(RECEIVER_ID);
  display.setCursor(0, 32);
  display.print("Lahan Terdaftar: ");
  display.println(getSenderCount());
  display.setCursor(0, 48);
  display.println("Tekan:Kembali ke Menu");
  display.display();
}

void showSystemInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== INFO SISTEM ===");
  display.setCursor(0, 12);
  display.print("Free RAM: ");
  display.print(ESP.getFreeHeap());
  display.println(" bytes");
  display.setCursor(0, 20);
  display.print("WiFi: ");
  display.println(isDashboardActive() ? "ON" : "OFF");
  display.setCursor(0, 28);
  display.print("LoRa RSSI: ");
  display.println(LoRa.packetRssi());
  display.setCursor(0, 48);
  display.println("Tekan:Kembali ke Menu");
  display.display();
}

void showDashboardInfo(String ip) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Web Aktif!");
  display.setCursor(0, 12);
  display.print("WiFi: ");
  display.println(DASHBOARD_SSID);
  display.setCursor(0, 20);
  display.print("Pass: ");
  display.println(DASHBOARD_PASSWORD);
  display.setCursor(0, 28);
  display.print("IP: ");
  display.println(ip);
  display.setCursor(0, 48);
  display.println("Tekan:Kembali ke Menu");
  display.display();
}

void showRequestingData(String senderID, int dots) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Meminta Data...");
  display.setCursor(0, 12);
  display.print("To: ");
  display.println(senderID);
  
  display.setCursor(0, 24);
  display.print("Menunggu");
  for (int j = 0; j <= dots; j++) {
    display.print(".");
  }
  display.display();
}