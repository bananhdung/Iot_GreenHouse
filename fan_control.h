#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include "config.h"

inline void initializeFan() {
  pinMode(FAN_RELAY_PIN, OUTPUT);
  digitalWrite(FAN_RELAY_PIN, LOW);
  fanLastChange = millis();
  Serial.println("[FAN] Initialized");
}

inline void controlCoolingFan() {
  unsigned long now = millis();
  if (fanOn && now - fanLastChange >= FAN_ON_TIME) {
    digitalWrite(FAN_RELAY_PIN, HIGH);
    fanOn = false;
    fanLastChange = now;
    Serial.println("[FAN] OFF");
  } else if (!fanOn && now - fanLastChange >= FAN_OFF_TIME) {
    digitalWrite(FAN_RELAY_PIN, LOW);
    fanOn = true;
    fanLastChange = now;
    Serial.println("[FAN] ON");
  }
}

#endif