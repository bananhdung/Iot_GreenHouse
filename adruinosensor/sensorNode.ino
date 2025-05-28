#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include "ScioSense_ENS160.h"
#include "config.h"
#include "sensors.h"
#include "loRaComms.h"
#include "buzzer.h"

static unsigned long lastRead = 300;
static bool resting = false;

inline bool runEvery(unsigned long interval, unsigned long *previous) {
  unsigned long now = millis();
  if (now - *previous >= interval) {
    *previous = now;
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  sensors_begin(); 
  buzzer_begin();
  lora_begin();

  if (!lora_getPacketCount()) {
    buzzer_pattern(500, 300, 3);
  }

  lastRead = millis();
}

inline void softReset() {
  asm volatile ("  jmp 0"); 
}

void loop() {
  if (resting) {
    delay(1000);
    return;
  }

  if (runEvery(SENSOR_INTERVAL, &lastRead)) {
    SensorData data = sensors_read();
    sensors_setEnvData(data.tempC, data.hum);
    sensors_log(&data);
    lora_send(&data);

    if (lora_getPacketCount() >= RESET_PACKET_COUNT) {
      Serial.println(F("[System] Reached 1000 packets, resting for 10 minutes"));
      lora_resetPacketCount();
      resting = true;
      buzzer_pattern(200, 200, 5);
      delay(REST_DURATION);
      resting = false;
      Serial.println(F("[System] Resuming operation"));
      softReset(); 
    }
  }
}
