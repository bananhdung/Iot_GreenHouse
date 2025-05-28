#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include "web.h"
#include "config.h"
#include "relay_control.h"

WebServer server(80);

void sendMutexError() { server.send(500, "text/plain", "Mutex not initialized or timeout"); }

void handleSensorData() {
  if (!sensorMutex || !xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) return sendMutexError();
  char json[128];
  snprintf(json, sizeof(json), "{\"temp\":%.1f,\"hum\":%.1f,\"light\":%d,\"aqi\":%d,\"tvoc\":%d,\"co2\":%d}",
           sensorTemp, sensorHum, sensorLight, sensorAQI, sensorTVOC, sensorCO2);
  xSemaphoreGive(sensorMutex);
  server.send(200, "application/json", json);
}

void handleRelayData() {
  if (!sensorMutex || !xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) return sendMutexError();
  char json[32] = "[";
  for (int i = 0; i < 6; i++) {
    strcat(json, i ? "," : "");
    strcat(json, relayStates[i] ? "1" : "0");
  }
  strcat(json, "]");
  xSemaphoreGive(sensorMutex);
  server.send(200, "application/json", json);
}

void handleSetRelay() {
  if (!server.hasArg("relay") || !server.hasArg("state")) return server.send(400, "text/plain", "Thiếu tham số");
  int relay = server.arg("relay").toInt();
  bool state = server.arg("state").toInt();
  if (mode != 1 || manualLocked || relay < 0 || relay >= 6) return server.send(403, "text/plain", "Chế độ thủ công hoặc khóa đang bật");
  if (!xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) return sendMutexError();
  controlRelay(relay, state);
  relayStates[relay] = state;
  notifyBlynkRelayChange(relay, state);
  xSemaphoreGive(sensorMutex);
  Serial.printf("[Web] Rơ-le %d: %s qua Web\n", relay, state ? "ON" : "OFF");
  server.send(200, "text/plain", "OK");
}

void handleSetMode() {
  if (!server.hasArg("mode")) return server.send(400, "text/plain", "Missing mode parameter");
  if (!xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) return sendMutexError();
  mode = server.arg("mode").toInt();
  notifyBlynkModeChange(mode);
  xSemaphoreGive(sensorMutex);
  server.send(200, "text/plain", "OK");
}

void handleSetTimer() {
  if (!sensorMutex || !server.hasArg("relay") || !server.hasArg("start") || !server.hasArg("end") || !server.hasArg("enabled"))
    return server.send(400, "text/plain", "Missing parameters");

  int relay = server.arg("relay").toInt();
  String start = server.arg("start"), end = server.arg("end");
  bool enabled = server.arg("enabled").toInt();
  if (relay < 0 || relay >= 6) return server.send(400, "text/plain", "Invalid relay");

  int sh, sm, eh, em;
  if (sscanf(start.c_str(), "%d:%d", &sh, &sm) != 2 || sscanf(end.c_str(), "%d:%d", &eh, &em) != 2)
    return server.send(400, "text/plain", "Invalid time");

  if (!xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) return sendMutexError();
  relayTimers[relay] = {sh, sm, eh, em, enabled};
  xSemaphoreGive(sensorMutex);
  server.send(200, "text/plain", "OK");
}

void handleScanWiFi() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleSetWiFi() {
  if (!server.hasArg("ssid") || !server.hasArg("password"))
    return server.send(400, "text/plain", "Missing SSID/password");
  String ssid = server.arg("ssid"), password = server.arg("password");
  saveWiFiCredentials(ssid.c_str(), password.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());
  wifiState = WIFI_CONNECTING;
  wifiConnectStart = millis();
  wifiRetryCount = 1;
  server.send(200, "text/plain", "Connecting...");
}

void handleStatus() {
  char json[128];
  snprintf(json, sizeof(json), "{\"wifi\":\"%s\",\"lora\":\"%s\",\"mode\":%d}",
           wifiState == WIFI_CONNECTED ? "Connected" : wifiState == WIFI_ACCESS_POINT ? "AP Mode" : "Disconnected",
           loraInitialized ? "Initialized" : "Not Initialized", mode);
  server.send(200, "application/json", json);
}

void initWebServer() {
  server.on("/", []() {
    String html = MAIN_PAGE;
    html.replace("@WIFI_STATUS@", wifiState == WIFI_CONNECTED ? "Connected" : wifiState == WIFI_ACCESS_POINT ? "AP Mode" : "Disconnected");
    html.replace("@LORA_STATUS@", loraInitialized ? "Initialized" : "Not Initialized");
    server.send(200, "text/html", html);
  });
  server.on("/sensors", handleSensorData);
  server.on("/relays", handleRelayData);
  server.on("/setrelay", handleSetRelay);
  server.on("/setmode", handleSetMode);
  server.on("/settimer", handleSetTimer);
  server.on("/scanwifi", handleScanWiFi);
  server.on("/setwifi", handleSetWiFi);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("[WEB] Started");
}

#endif