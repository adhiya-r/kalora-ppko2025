#include "sleep.h"
#include "sensors.h"
#include <esp_sleep.h>
#include <Arduino.h>

// Global variables
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: 
      Serial.println("Wakeup caused by external signal using RTC_IO"); 
      break;
    case ESP_SLEEP_WAKEUP_EXT1: 
      Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
      break;
    case ESP_SLEEP_WAKEUP_TIMER: 
      Serial.println("Wakeup caused by timer"); 
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: 
      Serial.println("Wakeup caused by touchpad"); 
      break;
    case ESP_SLEEP_WAKEUP_ULP: 
      Serial.println("Wakeup caused by ULP program"); 
      break;
    default: 
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); 
      break;
  }
}

void enterDeepSleep() {
  Serial.println("Entering deep sleep mode...");
  Serial.flush();
  
  // Power down sensors
  powerDownSensors();
  
  // Configure timer wakeup (tetap dipertahankan)
  esp_sleep_enable_timer_wakeup(SLEEP_TIME_SECONDS * 1000000ULL);
  
  // Configure external wakeup (LoRa DIO0)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1); // <<< TAMBAHKAN BARIS INI
  // GPIO_NUM_2 adalah GPIO_NUM dari LORA_DI0
  // 1 artinya wakeup saat sinyal HIGH
  
  esp_deep_sleep_start();
}