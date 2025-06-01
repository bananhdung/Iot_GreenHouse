#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "config.h"
#include "blynk.h"

#define AUTO_CONTROL_DURATION     (15 * 60 * 1000)
#define MAX_AUTO_CONTROL_DURATION (2 * 60 * 60 * 1000)
#define NIGHT_LIGHTING_DURATION   (4 * 60 * 60 * 1000)
#define NIGHT_START_HOUR          18
#define NIGHT_END_HOUR            6
#define AUTO_SWITCH_HOUR          23

struct Threshold {
  float lower, upper;
  int relaysLower[2], relaysUpper[2];
};

struct RelayAutoState {
  bool active;
  unsigned long startTime;
};

void notifyBlynkRelayChange(int relay, bool state);
void notifyBlynkModeChange(int newMode);

extern bool manualLocked;
extern String autoMessages[6];
RelayAutoState relayAutoStates[6] = {{false, 0}};
bool nightLightingActive = false;
unsigned long nightLightingStart = 0;

const Threshold thresholds[] = {
  {25, 38, {RELAY_AIR, -1}, {RELAY_FAN, -1}},
  {20, 80, {RELAY_HUMIDIFIER, -1}, {RELAY_FAN, -1}},
  {50, 100000, {RELAY_LIGHTING, -1}, {-1, -1}},
  {0, 100, {-1, -1}, {RELAY_AIR_PURIFIER, -1}},
  {0, 200, {-1, -1}, {RELAY_AIR_PURIFIER, -1}},
  {800, 1200, {RELAY_CO2_SYSTEM, -1}, {RELAY_AIR, -1}}
};

inline void initializeRelays() {
  Wire.begin();
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(0x00);
  if (Wire.endTransmission() == 0) {
    for (int i = 0; i < 6; i++) relayStates[i] = false;
    Serial.println("[PCF8574] Initialized, all relays OFF");
  } else Serial.println("[PCF8574] Init failed");
}

inline void controlRelay(int pin, bool state) {
  if (pin < 0 || pin >= 6 || relayStates[pin] == state) return;
  Wire.requestFrom(PCF8574_ADDRESS, 1);
  if (!Wire.available()) return (void)Serial.println("[PCF8574] Read failed");

  relayStates[pin] = state;
  relayOnTimes[pin] = state ? millis() : 0;
  relayOffTimes[pin] = state ? 0 : millis();

  uint8_t newState = 0;
  for (int i = 0; i < 6; i++) if (relayStates[i]) newState |= (1 << i);

  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(newState);
  if (Wire.endTransmission() == 0)
    Serial.printf("[RELAY] Pin %d %s\n", pin, state ? "BẬT" : "TẮT"), notifyBlynkRelayChange(pin, state);
  else Serial.println("[PCF8574] Write failed");
}

inline bool canTurnOnRelay(int pin) {
  return pin >= 0 && pin < 6 && (relayOffTimes[pin] == 0 || (millis() - relayOffTimes[pin] >= RELAY_MIN_ON));
}

inline bool shouldTurnOffRelay(int pin) {
  return pin >= 0 && pin < 6 && relayOnTimes[pin] > 0 && (millis() - relayOnTimes[pin] >= RELAY_MAX_ON);
}

inline bool isTimerActive(int relay) {
  if (relay < 0 || relay >= 6 || !relayTimers[relay].enabled) return false;
  time_t now; time(&now);
  struct tm* t = localtime(&now);
  if (!t) return false;
  int currMin = t->tm_hour * 60 + t->tm_min;
  int start = relayTimers[relay].startHour * 60 + relayTimers[relay].startMinute;
  int end = relayTimers[relay].endHour * 60 + relayTimers[relay].endMinute;
  return start <= end ? (currMin >= start && currMin < end) : (currMin >= start || currMin < end);
}

inline void handleNightLighting() {
  time_t now; struct tm t;
  time(&now); localtime_r(&now, &t);
  int hour = t.tm_hour;
  unsigned long nowMs = millis();
  bool isNight = (hour >= NIGHT_START_HOUR || hour < NIGHT_END_HOUR);

  if (isNight && !nightLightingActive && hour >= NIGHT_START_HOUR) {
    nightLightingActive = true;
    nightLightingStart = nowMs;
    controlRelay(RELAY_LIGHTING, true);
    autoMessages[RELAY_LIGHTING] = "Lighting bật tự động vào ban đêm.";
  }
  if (nightLightingActive && nowMs - nightLightingStart >= NIGHT_LIGHTING_DURATION) {
    nightLightingActive = false;
    controlRelay(RELAY_LIGHTING, false);
    autoMessages[RELAY_LIGHTING] = "Lighting tắt sau 4 giờ.";
  }
}

inline void checkAutoSwitch() {
  time_t now; struct tm t;
  time(&now); localtime_r(&now, &t);
  if (t.tm_hour == AUTO_SWITCH_HOUR && t.tm_min == 0) {
    mode = 0; manualLocked = true;
    notifyBlynkModeChange(0);
    autoMessages[0] = "Chuyển sang tự động lúc 23:00.";
  } else if (t.tm_hour != AUTO_SWITCH_HOUR) manualLocked = false;
}

inline void autoControl() {
  if (mode != 0 || !sensorMutex) return;
  if (!xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) return;

  unsigned long nowMs = millis();
  float sensors[] = {sensorTemp, sensorHum, sensorLight, sensorAQI, sensorTVOC, sensorCO2};
  String names[] = {"Temp", "Hum", "Light", "AQI", "TVOC", "CO2"};
  String relays[] = {"Lighting", "Fan", "Humidifier", "Air Purifier", "CO2 System", "Air"};

  for (int i = 0; i < 6; i++) autoMessages[i] = "";

  for (int i = 0; i < 6; i++) {
    float val = sensors[i]; const Threshold& th = thresholds[i];
    for (int j = 0; j < 2; j++) {
      int rLow = th.relaysLower[j], rHigh = th.relaysUpper[j];
      if (val < th.lower && rLow != -1 && !relayAutoStates[rLow].active && canTurnOnRelay(rLow)) {
        relayAutoStates[rLow] = {true, nowMs};
        controlRelay(rLow, true);
        autoMessages[rLow] = relays[rLow] + " ON do " + names[i] + " thấp (" + val + ")";
      }
      if (val > th.upper && rHigh != -1 && !relayAutoStates[rHigh].active && canTurnOnRelay(rHigh)) {
        relayAutoStates[rHigh] = {true, nowMs};
        controlRelay(rHigh, true);
        autoMessages[rHigh] = relays[rHigh] + " ON do " + names[i] + " cao (" + val + ")";
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    if (!relayAutoStates[i].active) continue;
    unsigned long elapsed = nowMs - relayAutoStates[i].startTime;
    bool inThresh = true;
    for (int j = 0; j < 6; j++) {
      if ((thresholds[j].relaysLower[0] == i || thresholds[j].relaysLower[1] == i) && sensors[j] < thresholds[j].lower) inThresh = false;
      if ((thresholds[j].relaysUpper[0] == i || thresholds[j].relaysUpper[1] == i) && sensors[j] > thresholds[j].upper) inThresh = false;
    }
    if (elapsed >= MAX_AUTO_CONTROL_DURATION || (elapsed >= AUTO_CONTROL_DURATION && inThresh)) {
      relayAutoStates[i].active = false;
      controlRelay(i, false);
      autoMessages[i] = relays[i] + " OFF sau thời gian auto.";
    }
  }

  xSemaphoreGive(sensorMutex);
}

inline void checkRelayTimers() {
  if (mode != 1 || manualLocked || !sensorMutex) return;
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      int currentMin = timeinfo.tm_hour * 60 + timeinfo.tm_min;
      for (int i = 0; i < 6; i++) {
        if (!relayTimers[i].enabled) continue;
        int startMin = relayTimers[i].startHour * 60 + relayTimers[i].startMinute;
        int endMin = relayTimers[i].endHour * 60 + relayTimers[i].endMinute;
        bool active = (startMin <= endMin)
                      ? (currentMin >= startMin && currentMin < endMin)
                      : (currentMin >= startMin || currentMin < endMin);
        if (relayStates[i] != active) {
          controlRelay(i, active);
          relayStates[i] = active;
          notifyBlynkRelayChange(i, active);
          Serial.printf("[Timer] Relay %d: %s\n", i, active ? "BẬT" : "TẮT");
        }
      }
    }
    xSemaphoreGive(sensorMutex);
  }
}


inline void controlRelaysAuto() {
  if (mode == 1 || !sensorMutex) return;
  if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    for (int i = 0; i < 6; i++) {
      bool desired = isTimerActive(i);
      if (desired && !relayStates[i] && canTurnOnRelay(i)) controlRelay(i, true);
      else if ((!desired && relayStates[i]) || (relayStates[i] && shouldTurnOffRelay(i))) controlRelay(i, false);
    }
    xSemaphoreGive(sensorMutex);
  } else Serial.println("[RELAY] Mutex timeout");

  autoControl();
  handleNightLighting();
}

inline void setupRelays() { initializeRelays(); }
inline void loopRelays()  { checkAutoSwitch(); controlRelaysAuto();  checkRelayTimers();  }

#endif
