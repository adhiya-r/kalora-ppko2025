#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include <Adafruit_SSD1306.h>

// Display object (extern to be shared)
extern Adafruit_SSD1306 display;

// Function declarations
bool initializeDisplay();
void showBootScreen();
void displayError(String message);
void showMainMenu();
void updateMenuDisplay();
void showSensorsList();
void showSensorData();
void showSettingsMenu();
void showSystemInfo();
void showDashboardInfo(String ip);
void showRequestingData(String senderID, int dots);

// Global variables for display
extern MenuState currentMenu;
extern int menuIndex;
extern const int maxMenuItems;

#endif