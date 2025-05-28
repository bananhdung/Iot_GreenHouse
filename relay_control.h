#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "config.h"
#include "blynk.h"

#define AUTO_CONTROL_DURATION_MS     (15UL * 60UL * 1000UL)
#define MAX_AUTO_CONTROL_DURATION (2UL * 60UL * 60UL * 1000UL)
#define NIGHT_LIGHTING_DURATION   (4UL * 60UL * 60UL * 1000UL)
#define NIGHT_START_HOUR      18
#define NIGHT_END_HOUR         6
#define AUTO_CONTROL_HOUR     23

struct Threshold {
  float lower, upper;
  int relaysLower[2], relaysUpper[2];
};

void notifyBlynkRelayChange(int pin, bool state);
void notifyBlynkModeChange(int newMode);

extern bool manualLocked;
extern String autoMessages[6];
extern RelayAutoState relayAutoStates[6];
extern bool nightLightingActive;
extern unsigned long nightLightingStart;
extern bool relayStates[6];
extern unsigned long relayOnTimes[6];
extern unsigned long relayOffTimes[6];
extern RelayTimer relayTimers[6];

const Threshold thresholds[] = {
  {25.0f, 38.0f, {RELAY_AIR, -1}, {RELAY_FAN, -1}},
  {20.0f, 80.0f, {RELAY_HUMIDIFIER, -1}, {RELAY_FAN, -1}},
  {50.0f, 100000.0f, {RELAY_LIGHTING, -1}, {-1, -1}},
  {0.0f, 100.0f, {-1, -1}, {RELAY_AIR_PURIFIER, -1}},
  {0.0f, 80.0f, {-1, -1}, {RELAY_AIR_PURIFIER, -1}}, // Độ mở rộng cho TVOC
  {800.0f, 1200.0f, {RELAY_CO2_SYSTEM, -1}, {RELAY_AIR, -1}}
};

inline void initializeRelays() {
  Wire.begin();
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(0x00);
  if (Wire.endTransmission() == 0) {
    for (int i = 0; i < 6; i++) relayStates[i] = false;
    Serial.println("[RELAY] PCF8574 khởi tạo, tất cả rơ-le tắt");
  } else Serial.println("[ERROR] PCF8574 khởi tạo thất bại");
}

inline void controlRelay(int pin, bool state) {
  if (pin < 0 || pin >= 6 || relayStates[pin] == state) return;
  if (!state && relayOnTimes[pin] > 0 && (millis() - relayOnTimes[pin] < 60000UL)) return; // Tối thiểu 1 phút bật
  Wire.requestFrom(PCF8574_ADDRESS, 1);
  if (!Wire.available()) return (void)Serial.println("[ERROR] PCF8574 đọc thất bại");

  relayStates[pin] = state;
  relayOnTimes[pin] = state ? millis() : 0;
  relayOffTimes[pin] = state ? 0 : millis();

  uint8_t newState = 0;
  for (int i = 0; i < 6; i++) if (relayStates[i]) newState |= (1 << i);

  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(newState);
  if (Wire.endTransmission() == 0) {
    Serial.printf("[RELAY] Pin %d %s\n", pin, state ? "BẬT" : "TẮT");
    notifyBlynkRelayChange(pin, state);
  } else Serial.println("[ERROR] PCF8574 ghi thất bại");
}

inline bool canTurnOnRelay(int pin) {
  return pin >= 0 && pin < 6 && (relayOffTimes[pin] == 0 || (millis() - relayOffTimes[pin] >= RELAY_MIN_ON));
}

inline bool shouldTurnOffRelay(int pin) {
  return pin >= 0 && pin < 6 && relayOnTimes[pin] > 0 && (millis() - relayOnTimes[pin] >= RELAY_MAX_ON);
}

inline void handleRelayControl() {
  if (!sensorMutex) return;
  if (!xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10))) {
    Serial.println("[ERROR] Mutex rơ-le timeout");
    return;
  }

  unsigned long nowMs = millis();
  float sensors[] = {sensorTemp, sensorHum, sensorLight, sensorAQI, sensorTVOC, sensorCO2};
  const char* names[] = {"Nhiệt độ", "Độ ẩm", "Ánh sáng", "AQI", "TVOC", "CO2"};
  const char* relays[] = {"Điều hòa", "Quạt", "Máy tạo ẩm", "Đèn", "Máy lọc không khí", "Hệ thống CO2"};

  if (mode == 0) {
    for (int i = 0; i < 6; i++) autoMessages[i] = "";
    
    for (int i = 0; i < 6; i++) {
      float val = sensors[i];
      const Threshold& th = thresholds[i];
      for (int j = 0; j < 2; j++) {
        int rLow = th.relaysLower[j], rHigh = th.relaysUpper[j];
        if (val < th.lower && rLow != -1 && !relayAutoStates[rLow].active && canTurnOnRelay(rLow)) {
          relayAutoStates[rLow] = {true, nowMs};
          controlRelay(rLow, true);
          autoMessages[rLow] = String(relays[rLow]) + " ON do " + names[i] + " thấp (" + String(val, 1) + ")";
        }
        if (val > th.upper && rHigh != -1 && !relayAutoStates[rHigh].active && canTurnOnRelay(rHigh)) {
          relayAutoStates[rHigh] = {true, nowMs};
          controlRelay(rHigh, true);
          autoMessages[rHigh] = String(relays[rHigh]) + " ON do " + names[i] + " cao (" + String(val, 1) + ")";
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
      if (elapsed >= MAX_AUTO_CONTROL_DURATION || (elapsed >= AUTO_CONTROL_DURATION_MS && inThresh)) {
        relayAutoStates[i].active = false;
        controlRelay(i, false);
        autoMessages[i] = String(relays[i]) + " OFF sau tự động.";
      }
    }

    time_t now; struct tm t;
    time(&now); localtime_r(&now, &t);
    int hour = t.tm_hour;
    bool isNight = (hour >= NIGHT_START_HOUR || hour < NIGHT_END_HOUR);
    if (isNight && !nightLightingActive && hour >= NIGHT_START_HOUR) {
      nightLightingActive = true;
      nightLightingStart = nowMs;
      controlRelay(RELAY_LIGHTING, true);
      autoMessages[RELAY_LIGHTING] = "Đèn bật tự động vào ban đêm.";
    }
    if (nightLightingActive && nowMs - nightLightingStart >= NIGHT_LIGHTING_DURATION) {
      nightLightingActive = false;
      controlRelay(RELAY_LIGHTING, false);
      autoMessages[RELAY_LIGHTING] = "Đèn tắt sau 4 giờ.";
    }
  }

  if (mode == 1 && !manualLocked) {
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
          autoMessages[i] = String(relays[i]) + (active ? " ON do hẹn giờ" : " OFF do hẹn giờ");
        }
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    if (relayStates[i] && shouldTurnOffRelay(i)) {
      controlRelay(i, false);
      autoMessages[i] = String(relays[i]) + " OFF do vượt thời gian tối đa.";
    }
  }

  xSemaphoreGive(sensorMutex);
}

inline void checkAutoSwitch() {
  time_t now; struct tm t;
  time(&now); localtime_r(&now, &t);
  if (t.tm_hour == AUTO_CONTROL_HOUR && t.tm_min == 0) {
    mode = 0; manualLocked = true;
    notifyBlynkModeChange(0);
    autoMessages[0] = "Chuyển sang tự động lúc 23:00.";
  } else if (t.tm_hour != AUTO_CONTROL_HOUR) manualLocked = false;
}

inline void setupRelays() { initializeRelays(); }

inline void loopRelays() {
  checkAutoSwitch();
  handleRelayControl();
}

#endif