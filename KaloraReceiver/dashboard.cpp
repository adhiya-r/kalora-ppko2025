#include "dashboard.h"
#include "display.h"
#include "data.h"
#include <WiFi.h>
#include <ESPmDNS.h>

// Global variables
WebServer server(WEB_SERVER_PORT);
bool dashboardMode = false;

void initializeDashboard() {
  dashboardMode = false;
}

void toggleDashboardMode() {
  if (!dashboardMode) {
    startDashboard();
    Serial.println("Dashboard Mode On");
  } else {
    stopDashboard();
    Serial.println("Dashboard Mode Off");
  }
}

void showDashboardInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== DASHBOARD ===");
  display.setCursor(0, 12);
  display.print("Status: ");
  display.println(dashboardMode ? "ACTIVE" : "INACTIVE");

  if (dashboardMode) {
    display.setCursor(0, 20);
    display.print("SSID: ");
    display.println(DASHBOARD_SSID);
    display.setCursor(0, 28);
    display.print("IP: ");
    display.println(WiFi.softAPIP().toString()); // Gunakan IP AP
  }
  
  display.setCursor(0, 56);
  display.println("SW: Back to Menu");
  display.display();
}

void startDashboard() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Starting Dashboard...");
  display.display();
  
  WiFi.softAP(DASHBOARD_SSID, DASHBOARD_PASSWORD);
  IPAddress IP = WiFi.softAPIP();
  
  server.on("/", handleRoot);
  server.on("/api/data", handleAPIData);
  server.begin();

  // Inisialisasi mDNS
  if (!MDNS.begin("kalora")) { // Ganti "kalora" dengan nama yang Anda inginkan
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println("mDNS responder started");
  Serial.println("You can now access the dashboard at http://kalora.local");

  // Tambahkan layanan HTTP ke mDNS
  MDNS.addService("http", "tcp", WEB_SERVER_PORT);
  
  dashboardMode = true;
  
  showDashboardInfo(IP.toString());
  delay(3000);
}

void stopDashboard() {
  server.stop();
  WiFi.softAPdisconnect(true);
  MDNS.end();
  dashboardMode = false;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 24);
  display.println("Dashboard Stopped");
  display.display();
  delay(2000);
}

void handleDashboardClient() {
  if (dashboardMode) {
    server.handleClient();
  }
}

bool isDashboardActive() {
  return dashboardMode;
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Kalora Dashboard</title>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{font-family:Arial;margin:20px;background:#f0f0f0}";
  html += ".container{max-width:800px;margin:0 auto;background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1)}";
  html += ".sensor-card{background:#e8f5e8;padding:15px;margin:10px 0;border-radius:8px;border-left:4px solid #4CAF50}";
  html += ".value{font-size:1.5em;font-weight:bold;color:#2E7D32}</style>";
  html += "<script>setInterval(function(){location.reload();},5000);</script></head><body>";
  html += "<div class='container'><h1>🌳 Kalora Forest Monitoring</h1>";
  
  SensorData* latestData = getLatestData();
  
  if (latestData->valid) {
    html += "<div class='sensor-card'>";
    html += "<h3>Sensor: " + latestData->senderID + "</h3>";
    html += "<p>Temperature: <span class='value'>" + String(latestData->temperature, 1) + "°C</span></p>";
    html += "<p>Soil Moisture: <span class='value'>" + String(latestData->soilMoisture, 1) + "%</span></p>";
    html += "<p>Light Intensity: <span class='value'>" + String(latestData->lightIntensity, 0) + " lux</span></p>";
    html += "<p><small>Last update: " + String((millis() - latestData->timestamp) / 1000) + " seconds ago</small></p>";
    html += "</div>";
  } else {
    html += "<div class='sensor-card'>";
    html += "<p>No sensor data available. Please request data from the device menu.</p>";
    html += "</div>";
  }
  
  html += "<p><small>Auto-refresh every 5 seconds</small></p>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

void handleAPIData() {
  String json = "{";
  SensorData* latestData = getLatestData();
  
  if (latestData->valid) {
    json += "\"senderID\":\"" + latestData->senderID + "\",";
    json += "\"temperature\":" + String(latestData->temperature, 1) + ",";
    json += "\"soilMoisture\":" + String(latestData->soilMoisture, 1) + ",";
    json += "\"lightIntensity\":" + String(latestData->lightIntensity, 0) + ",";
    json += "\"timestamp\":" + String(latestData->timestamp) + ",";
    json += "\"valid\":true";
  } else {
    json += "\"valid\":false";
  }
  json += "}";
  
  server.send(200, "application/json", json);
}