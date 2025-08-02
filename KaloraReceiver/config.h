#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==================== Pin definitions for LoRa ====================
#define LORA_SCK     18     // D18
#define LORA_MISO    19     // D19
#define LORA_MOSI    23     // D23
#define LORA_SS      5      // D5 (NSS)
#define LORA_RST     14     // D14
#define LORA_DI0     2      // D2

// ==================== Pin definitions for OLED (I2C) ====================
#define OLED_SDA 21     // D21
#define OLED_SCL 22     // D22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1   // Not connected (default)

// ==================== Pin definitions for Joystick ====================
#define JOYSTICK_VRX 34     // Analog X
#define JOYSTICK_VRY 35     // Analog Y
#define JOYSTICK_SW  27     // Button

// ==================== System configuration ====================
#define RECEIVER_ID "RC0501"
#define LORA_FREQUENCY 915E6
#define JOYSTICK_DEADZONE 500
#define JOYSTICK_DEBOUNCE_MS 200
#define DATA_REQUEST_TIMEOUT 3000

// ==================== Data structures ====================
struct SensorData {
  String senderID;
  float temperature;
  float soilMoisture;
  float lightIntensity;
  unsigned long timestamp;
  bool valid;
};

struct RegisteredSender {
  String id;
  String description;
  bool active;
};

// ==================== Menu system ====================
enum MenuState {
  MENU_MAIN,
  MENU_SENSORS,
  MENU_REQUEST_DATA,
  MENU_VIEW_DATA,
  MENU_SETTINGS,
  MENU_DASHBOARD,
  MENU_SYSTEM_INFO
};

// ==================== WiFi Dashboard configuration ====================
#define DASHBOARD_SSID "Kalora_Dashboard"
#define DASHBOARD_PASSWORD "kalora123"
#define WEB_SERVER_PORT 80

#endif