#include "input.h"
#include "display.h"
#include "loracomms.h"
#include "dashboard.h"
#include "data.h"
#include <Arduino.h>

// Joystick variables
int joystickCenterX = 2048;
int joystickCenterY = 2048;
// lastJoystickMoveTime untuk debounce gerakan (X/Y)
unsigned long lastJoystickMoveTime = 0; // Ubah nama variabel ini

// Debounce variables untuk tombol SW
bool lastButtonState = HIGH; // HIGH = tidak ditekan (pull-up)
unsigned long lastButtonPressTime = 0; // Waktu terakhir tombol berubah status (tekan/lepas)
bool buttonClickProcessed = false; // Flag untuk memastikan klik hanya diproses sekali per penekanan

void initializeJoystick() {
  pinMode(JOYSTICK_SW, INPUT_PULLUP);
  // Kalibrasi tengah joystick
  joystickCenterX = analogRead(JOYSTICK_VRX);
  joystickCenterY = analogRead(JOYSTICK_VRY);
  Serial.println("Joystick calibrated."); // Debug: pastikan ini terpanggil
  Serial.print("Center X: "); Serial.println(joystickCenterX);
  Serial.print("Center Y: "); Serial.println(joystickCenterY);
}

void handleJoystickInput() {
  unsigned long currentTime = millis(); // Ambil waktu saat ini sekali

  // --- BACA SEMUA INPUT ---
  int x = analogRead(JOYSTICK_VRX);
  int y = analogRead(JOYSTICK_VRY);
  bool currentButtonState = digitalRead(JOYSTICK_SW); // Baca status tombol saat ini
  bool buttonCurrentlyPressed = !currentButtonState; // true jika tombol ditekan (LOW)

  // --- HANDLE TOMBOL KLIK (SW) ---
  // Perubahan status tombol (untuk debounce)
  if (currentButtonState != lastButtonState) {
    lastButtonPressTime = currentTime; // Catat waktu perubahan
    buttonClickProcessed = false; // Reset flag proses klik saat ada perubahan status
  }
  lastButtonState = currentButtonState; // Update status tombol terakhir

  // Cek apakah tombol sedang ditekan, sudah melewati waktu debounce, dan belum diproses
  if (buttonCurrentlyPressed && (currentTime - lastButtonPressTime) > JOYSTICK_DEBOUNCE_MS && !buttonClickProcessed) {
    // Tombol SW DITEKAN dan SIAP DIPROSES (valid click)
    Serial.println("SW CLICK DETECTED!"); // Debug

    if (currentMenu == MENU_MAIN) {
      handleMenuSelection(); // Proses pilihan menu utama
    } else {
      showMainMenu(); // Di sub-menu, ini berarti kembali
    }
    
    // Set flag bahwa klik sudah diproses
    buttonClickProcessed = true; 
    // lastJoystickMoveTime juga bisa di-reset untuk memberi jeda pada gerakan setelah klik
    lastJoystickMoveTime = currentTime;
    
    // Penting: Jangan return di sini. Biarkan logika gerakan tetap dievaluasi
    // jika tombol ditekan sambil digerakkan, atau untuk memastikan debounce lain bekerja
  }

  // --- HANDLE GERAKAN JOYSTICK ---
  // Hanya proses gerakan jika sudah melewati debounce time untuk gerakan
  if ((currentTime - lastJoystickMoveTime) < JOYSTICK_DEBOUNCE_MS) {
    return; // Keluar jika masih dalam periode debounce gerakan
  }

  bool upPressed = (y > joystickCenterY + JOYSTICK_DEADZONE);
  bool downPressed = (y < joystickCenterY - JOYSTICK_DEADZONE);
  bool leftPressed = (x < joystickCenterX - JOYSTICK_DEADZONE);
  bool rightPressed = (x > joystickCenterX + JOYSTICK_DEADZONE);

  bool menuChanged = false;

  if (upPressed || downPressed) { // Cek hanya jika ada pergerakan vertikal
    int actualMaxMenuItems = 0;
    switch (currentMenu) {
      case MENU_MAIN: actualMaxMenuItems = 6; break;
      case MENU_SENSORS: actualMaxMenuItems = getSenderCount(); break;
      case MENU_SETTINGS: actualMaxMenuItems = 1; break;
      case MENU_DASHBOARD: actualMaxMenuItems = 1; break;
      case MENU_SYSTEM_INFO: actualMaxMenuItems = 1; break;
      case MENU_VIEW_DATA: actualMaxMenuItems = 1; break;
      case MENU_REQUEST_DATA: actualMaxMenuItems = 1; break;
      default: actualMaxMenuItems = 1; break;
    }

    if (upPressed) {
      menuIndex = (menuIndex - 1 + actualMaxMenuItems) % actualMaxMenuItems;
    } else { // downPressed
      menuIndex = (menuIndex + 1) % actualMaxMenuItems;
    }
    menuChanged = true;
    lastJoystickMoveTime = currentTime; // Reset waktu gerakan setelah diproses
  }
  // Anda bisa menambahkan logika untuk leftPressed/rightPressed di sini jika ingin

  if (menuChanged) {
    updateMenuDisplay();
  }
}

// Catatan: Fungsi handleMenuSelection() sekarang hanya memproses pilihan di MENU_MAIN.
// Fungsi showMainMenu() akan menjadi cara standar untuk kembali.

void handleMenuSelection() {
  switch (currentMenu) {
    case MENU_MAIN:
      switch (menuIndex) {
        case 0: // View Sensors
          currentMenu = MENU_SENSORS;
          menuIndex = 0; // Reset index untuk sub-menu jika diperlukan
          updateMenuDisplay();
          break;

        case 1: // Request Data
          // Ini akan menjadi non-blocking, jadi panggil fungsi untuk memulai request
          // dan set currentMenu ke state "requesting"
          // requestDataFromSender() // Akan diubah nanti menjadi non-blocking
          // Untuk saat ini, kita bisa buat state waiting
          currentMenu = MENU_REQUEST_DATA; // Kita tambahkan state ini di enum
          requestDataFromSender(getSendersArray()[0].id);
          updateMenuDisplay(); // Tampilkan layar "Requesting Data..."
          // Catatan: logika pengiriman permintaan LoRa dan timeout akan ada di loop() utama atau fungsi terpisah
          break;

        case 2: // View Last Data
          currentMenu = MENU_VIEW_DATA;
          updateMenuDisplay();
          break;

        case 3: // Settings
          currentMenu = MENU_SETTINGS; // Pindah ke state settings
          updateMenuDisplay();
          break;

        case 4: // Dashboard Mode
          // Panggil fungsi toggle
          toggleDashboardMode(); // Ini tidak lagi memanggil showMainMenu()
          // Setelah toggle, set menu ke state Dashboard
          currentMenu = MENU_DASHBOARD;
          updateMenuDisplay(); // Tampilkan info Dashboard
          break;

        case 5: // System Info
          currentMenu = MENU_SYSTEM_INFO; // Pindah ke state system info
          updateMenuDisplay();
          break;
      }
    break;
  }
}