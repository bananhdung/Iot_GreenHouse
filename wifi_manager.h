#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Preferences.h>
#include "config.h"
#include "blynk.h"

#define MAX_WIFI_RETRIES      5
#define WIFI_TIMEOUT          5000
#define AP_SSID               "GreenHouse"
#define AP_PASSWORD           "12345678"

extern String ssid, password;
static Preferences preferences;
static int wifiRetryCount = 0;

void saveWiFiCredentials(const char* ssid, const char* password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
  Serial.printf("[NVS] Saved WiFi: %s\n", ssid);
}

void loadWiFiCredentials(String& ssid, String& password) {
  preferences.begin("wifi", true);
  ssid = preferences.getString("ssid", ssid);
  password = preferences.getString("password", password);
  preferences.end();
  Serial.printf("[NVS] Loaded WiFi: %s\n", ssid.c_str());
}

void startAP(const String& ssid = AP_SSID, const String& password = AP_PASSWORD) {
  WiFi.softAP(ssid.c_str(), password.c_str());
  Serial.println("[WIFI] AP started: " + WiFi.softAPIP().toString());
}

void initWiFi() {
  WiFi.mode(WIFI_MODE_APSTA);
  startAP(); 
  Serial.println("[WIFI] AP ON - IP: " + WiFi.softAPIP().toString());

  loadWiFiCredentials(ssid, password);

  Serial.printf("[WIFI] Connecting to: %s\n", ssid.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());
  wifiState = WIFI_CONNECTING;
  wifiRetryCount = 0;
  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && wifiRetryCount < MAX_WIFI_RETRIES) {
    if (millis() - startTime >= WIFI_TIMEOUT) {
      wifiRetryCount++;
      Serial.printf("[WIFI] Retry %d/%d\n", wifiRetryCount, MAX_WIFI_RETRIES);
      WiFi.disconnect();
      WiFi.begin(ssid.c_str(), password.c_str());
      startTime = millis();
    }
    delay(500); 
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiState = WIFI_CONNECTED;
    Serial.printf("[WIFI] Connected: %s\n", WiFi.localIP().toString().c_str());
    configTime(7 * 3600, 0, "pool.ntp.org"); 
    initializeBlynk(); 
  } else {
    wifiState = WIFI_IDLE;
    Serial.println("[WIFI] Connection failed, staying in AP mode");
    blynkConnected = false;
  }
}

#endif