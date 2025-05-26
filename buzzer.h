#ifndef BUZZER_H
#define BUZZER_H

#include "config.h"

inline void buzzer_control(bool state) {
  digitalWrite(BUZZER_PIN, state ? HIGH : LOW);
}

inline void buzzer_begin() {
  pinMode(BUZZER_PIN, OUTPUT);
  buzzer_control(false);
}

inline void buzzer_pattern(int onTime, int offTime, int repeatCount) {
  for (int i = 0; i < repeatCount; i++) {
    buzzer_control(true);
    delay(onTime);
    buzzer_control(false);
    delay(offTime);
  }
}

#endif