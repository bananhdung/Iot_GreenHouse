#ifndef LORA_COMMS_H
#define LORA_COMMS_H

#include "sensors.h"
#include "config.h"
#include <SPI.h>
#include <LoRa.h>

static unsigned long packetCount = 0;
static bool loraInitialized = false;
static SensorData lastData = {0, 0, 0, 0, 0, 0}; // Khai báo lastData
static unsigned long lastSend = 0; // Khai báo lastSend

inline void lora_begin() {
  LoRa.setPins(csPin, resetPin, irqPin);
  int attempt = 0, maxRetries = 5;

  while (!LoRa.begin(frequency) && attempt < maxRetries) {
    char buf[32];
    snprintf(buf, sizeof(buf), "[LoRa] Attempt %d", attempt + 1);
    Serial.println(buf);
    delay(1000);
    attempt++;
  }

  if (attempt < maxRetries) {
    loraInitialized = true;
    LoRa.setSpreadingFactor(7);         // Thêm để tương thích với bên nhận
    LoRa.setSignalBandwidth(125E3);     // Thêm để tương thích với bên nhận
    LoRa.enableCrc();                   // Thêm để tăng độ tin cậy
    LoRa.disableInvertIQ();
    Serial.println(F("[LoRa] Initialized"));
  } else {
    loraInitialized = false;
    Serial.println(F("[LoRa] Initialization failed"));
  }
}

inline void LoRa_txMode() {
  LoRa.idle();
  LoRa.disableInvertIQ();
}

inline bool hasSignificantChange(const SensorData *current) {
  if (packetCount == 0) return true;

  float deltaTemp = abs(current->tempC - lastData.tempC) / (lastData.tempC == 0 ? 1 : lastData.tempC);
  float deltaHum = abs(current->hum - lastData.hum) / (lastData.hum == 0 ? 1 : lastData.hum);
  float deltaLight = abs(current->lightLvl - lastData.lightLvl) / (lastData.lightLvl == 0 ? 1 : lastData.lightLvl);
  float deltaAQI = abs(current->aqi - lastData.aqi) / (lastData.aqi == 0 ? 1 : lastData.aqi);
  float deltaTVOC = abs(current->tvoc - lastData.tvoc) / (lastData.tvoc == 0 ? 1 : lastData.tvoc);
  float deltaECO2 = abs(current->eco2 - lastData.eco2) / (lastData.eco2 == 0 ? 1 : lastData.eco2);

  bool significant = deltaTemp >= CHANGE_PERCENT_TEMP ||
                     deltaHum >= CHANGE_PERCENT_HUM ||
                     deltaLight >= CHANGE_PERCENT_LIGHT ||
                     deltaAQI >= CHANGE_PERCENT_OTHER ||
                     deltaTVOC >= CHANGE_PERCENT_OTHER ||
                     deltaECO2 >= CHANGE_PERCENT_OTHER;

  if (significant) {
    lastData = *current;
  }

  return significant;
}

inline bool LoRa_sendMessage(String message) {
  LoRa_txMode();
  LoRa.beginPacket();
  LoRa.print(message);
  return LoRa.endPacket(true);
}

inline void lora_send(const SensorData *data) {
  if (!loraInitialized) return;

  bool significantChange = hasSignificantChange(data);
  if (!significantChange && (millis() - lastSend < 10000)) {
    return;
  }

  char payload[32];
  int tempC_int = (int)(data->tempC * 10);
  int hum_int = (int)(data->hum * 10);
  snprintf(payload, sizeof(payload), "S_%d.%d,%d.%d,%d,%d,%d,%d",
           tempC_int / 10, tempC_int % 10, hum_int / 10, hum_int % 10,
           data->lightLvl, data->aqi, data->tvoc, data->eco2);

  Serial.print(F("[LoRa] Sending: "));
  Serial.println(payload);

  if (LoRa_sendMessage(payload)) {
    packetCount++;
    lastSend = millis(); // Cập nhật lastSend sau khi gửi thành công
    Serial.print(F("[LoRa] Sent successfully. Packet #"));
    Serial.println(packetCount);
  } else {
    Serial.println(F("[LoRa] Failed to send packet"));
  }
}

inline unsigned long lora_getPacketCount() {
  return packetCount;
}

inline void lora_resetPacketCount() {
  packetCount = 0;
}

#endif