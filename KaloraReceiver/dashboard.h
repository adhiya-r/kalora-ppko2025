#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "config.h"
#include <WiFi.h>
#include <WebServer.h>

// Function declarations
void initializeDashboard();
void toggleDashboardMode();
void showDashboardInfo();
void startDashboard();
void stopDashboard();
void handleDashboardClient();
extern bool isDashboardActive();


// Web handlers
void handleRoot();
void handleAPIData();

// Global variables
extern WebServer server;
extern bool dashboardMode;

#endif