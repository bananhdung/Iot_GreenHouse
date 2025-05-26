#ifndef BLYNK_H
#define BLYNK_H

#include <WiFi.h>
#include <ArduinoJson.h>
#include "config.h"
#include "relay_control.h"

// Blynk Config
#define BLYNK_TEMPLATE_ID "TMPL6x9z1zKQW"
#define BLYNK_TEMPLATE_NAME "ESP32"
#define BLYNK_AUTH_TOKEN "6Mt-nm7qmf_62JfNUtDExlLvBMfPjQP1"
#include <BlynkSimpleEsp32.h>

// External Variables
extern float sensorTemp, sensorHum;
extern int sensorLight, sensorAQI, sensorTVOC, sensorCO2;
extern bool relayStates[6];
extern int mode;
extern SemaphoreHandle_t sensorMutex;
extern RelayTimer relayTimers[6];
extern bool manualLocked;

// Internal Variables
BlynkTimer timer;
bool blynkConnected = false;
const unsigned long BLYNK_SEND_INTERVAL = 5000;

// Sync to Blynk
void syncToBlynk(bool fullSync = true) {
  if (!blynkConnected || !sensorMutex) return;
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    if (fullSync) {
      Blynk.virtualWrite(V0, sensorTemp);
      Blynk.virtualWrite(V1, sensorHum);
      Blynk.virtualWrite(V2, sensorLight);
      Blynk.virtualWrite(V3, sensorAQI);
      Blynk.virtualWrite(V4, sensorTVOC);
      Blynk.virtualWrite(V5, sensorCO2);
      Blynk.virtualWrite(V12, mode == 1 ? "MANUAL" : "AUTO");
      Blynk.virtualWrite(V13, mode);
    }
    for (int i = 0; i < 6; i++) Blynk.virtualWrite(V20 + i, relayStates[i]);
    Serial.println("[Blynk] Đồng bộ thành công");
    xSemaphoreGive(sensorMutex);
  } else Serial.println("[Blynk] Timeout Mutex");
}

void initializeBlynk() {
  Serial.println("[Blynk] Init");
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid.c_str(), password.c_str());
  timer.setInterval(BLYNK_SEND_INTERVAL, []() { syncToBlynk(); });
  Serial.println("[Blynk] Setup done");
}

void notifyBlynkRelayChange(int relay, bool state) {
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    relayStates[relay] = state;
    syncToBlynk(false);
    xSemaphoreGive(sensorMutex);
    Serial.printf("[Blynk] Relay %d: %s\n", relay, state ? "ON" : "OFF");
  }
}

void notifyBlynkModeChange(int newMode) {
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    mode = newMode;
    syncToBlynk();
    xSemaphoreGive(sensorMutex);
    Serial.printf("[Blynk] Mode: %s\n", mode == 1 ? "MANUAL" : "AUTO");
  }
}

BLYNK_WRITE_DEFAULT() {
  int pin = request.pin;
  if (pin >= V20 && pin <= V25 && mode == 1 && !manualLocked) {
    int relay = pin - V20;
    int value = param.asInt();
    if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
      controlRelay(relay, value);
      relayStates[relay] = value;
      syncToBlynk(false);
      xSemaphoreGive(sensorMutex);
      Serial.printf("[Blynk] Relay %d: %s\n", relay, value ? "ON" : "OFF");
    }
  } else Serial.println("[Blynk] Relay control locked or auto mode");
}

BLYNK_WRITE(V12) {
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    String value = param.asStr();
    mode = (value == "MANUAL") ? 1 : (value == "AUTO") ? 0 : mode;
    syncToBlynk();
    xSemaphoreGive(sensorMutex);
    Serial.printf("[Blynk] State: %s\n", value.c_str());
  }
}

BLYNK_WRITE(V13) {
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    mode = param.asInt();
    syncToBlynk();
    xSemaphoreGive(sensorMutex);
    Serial.printf("[Blynk] Mode: %s\n", mode == 1 ? "MANUAL" : "AUTO");
  }
}

bool connectToBlynk() {
  if (!Blynk.connected()) {
    Serial.println("[Blynk] Connecting...");
    if (Blynk.connect()) {
      Serial.println("[Blynk] Connected");
      blynkConnected = true;
      syncToBlynk();
    } else {
      Serial.println("[Blynk] Failed to connect");
      blynkConnected = false;
      return false;
    }
  }
  return true;
}

void loopBlynk() {
  if (wifiState != WIFI_CONNECTED) {
    blynkConnected = false;
    Blynk.disconnect();
    return;
  }
  if (!Blynk.connected()) connectToBlynk();
  Blynk.run();
  timer.run();
}

#endif
