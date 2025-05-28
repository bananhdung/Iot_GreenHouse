#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_AHTX0.h>
#include <ScioSense_ENS160.h>
#include "config.h"
#include <Wire.h>

typedef struct {
  float tempC;
  float hum;
  int lightLvl;
  int aqi;
  int tvoc;
  int eco2;
} SensorData;

static ScioSense_ENS160 ens160(ENS160_I2CADDR_1);
static Adafruit_AHTX0 aht;

inline void sensors_begin() {
  Serial.println(F("Initializing ENS160..."));
  if (!ens160.begin()) {
    Serial.println(F("[Error] ENS160 failed to start"));
  } else {
    ens160.setMode(ENS160_OPMODE_STD);
  }

  Serial.println(F("Initializing AHTX0..."));
  if (!aht.begin()) {
    Serial.println(F("[Error] AHTX0 failed to start"));
  }
}

inline SensorData sensors_read() {
  SensorData data = {0, 0, 0, 0, 0, 0};
  sensors_event_t humidity, temperature;
  aht.getEvent(&humidity, &temperature);

  if (!isnan(temperature.temperature)) {
    data.tempC = temperature.temperature * CALIBRATION_TEMP;
    if (data.tempC < 0 || data.tempC > 50) data.tempC = 0;
  }

  if (!isnan(humidity.relative_humidity)) {
    data.hum = humidity.relative_humidity * CALIBRATION_HUM;
    if (data.hum < 0 || data.hum > 100) data.hum = 0;
  }

  data.lightLvl = analogRead(TEMT6000_PIN) * CALIBRATION_LIGHT;
  if (data.lightLvl < 0 || data.lightLvl > 4095) data.lightLvl = 0;

  if (ens160.available()) {
    ens160.measure(true);
    data.aqi = ens160.getAQI() * CALIBRATION_AQI;
    if (data.aqi < 0 || data.aqi > 500) data.aqi = 0;
    data.tvoc = ens160.getTVOC() * CALIBRATION_TVOC;
    if (data.tvoc < 0 || data.tvoc > 5000) data.tvoc = 0;
    data.eco2 = ens160.geteCO2() * CALIBRATION_ECO2;
    if (data.eco2 < 0 || data.eco2 > 5000) data.eco2 = 0;
  }

  return data;
}

inline void sensors_setEnvData(float temp, float hum) {
  ens160.set_envdata(temp, hum);
}

inline void sensors_log(const SensorData *data) {
  char buf[64];
  int tempC_int = (int)(data->tempC * 10);  // 25.5 -> 255
  int hum_int = (int)(data->hum * 10);      // 60.2 -> 602
  snprintf(buf, sizeof(buf), "Data: T=%d.%dC H=%d.%d%% Light=%d AQI=%d TVOC=%d eCO2=%d",
           tempC_int / 10, tempC_int % 10, hum_int / 10, hum_int % 10,
           data->lightLvl, data->aqi, data->tvoc, data->eco2);
  Serial.println(buf);
}
#endif