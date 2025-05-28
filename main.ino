#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "relay_control.h"
#include "fan_control.h"
#include "lora_manager.h"
#include "web_server.h"
#include "blynk.h"

bool loraInitialized = false;
unsigned long fanLastChange = 0;
bool fanOn = true;
float sensorTemp = 0.0;
float sensorHum = 0.0;
int sensorLight = 0;
int sensorAQI = 0;
int sensorTVOC = 0;
int sensorCO2 = 0;
char packetBuffer[32] = {0};
int packetSize = 0;
int countpacket = 0;
SemaphoreHandle_t sensorMutex = nullptr;
int mode = 0;
bool relayStates[6] = {false, false, false, false, false, false};
unsigned long relayOnTimes[6] = {0, 0, 0, 0, 0, 0};
unsigned long relayOffTimes[6] = {0, 0, 0, 0, 0, 0};
bool manualLocked = false;
String autoMessages[6] = {"", "", "", "", "", ""};
RelayAutoState relayAutoStates[6] = {{false, 0}, {false, 0}, {false, 0}, {false, 0}, {false, 0}, {false, 0}};
bool nightLightingActive = false;
unsigned long nightLightingStart = 0;
RelayTimer relayTimers[6] = {{0, 0, 0, 0, false}};
WiFiState wifiState = WIFI_IDLE;
unsigned long wifiConnectStart = 0;

String ssid = "Ban Van Tot";
String password = "nhuquynh20111980";

void setup() {
  Serial.begin(9600);
  delay(100);

  sensorMutex = xSemaphoreCreateMutex();
  if (!sensorMutex) {
    Serial.println("[ERROR] Mutex creation failed");
    while (1);
  }
  Serial.println("[MUTEX] Initialized");
  setupRelays();
  initializeFan();
  initializeLoRa();
  initWiFi();
  initWebServer();
  Serial.println("[SETUP] Ready");
}

void loop() {
  controlCoolingFan();
  if (LoRa.parsePacket()) {
    processLoRaPacket(LoRa.available());
  }
  loopRelays();
  server.handleClient();
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck >= 10000) {
    if (!loraInitialized) {
      initializeLoRa();
    }
    lastCheck = millis();
  }
  loopBlynk();
}