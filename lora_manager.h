#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <LoRa.h>
#include <SPI.h>
#include "config.h"

extern char packetBuffer[32];
extern int packetSize, countpacket;
extern bool loraInitialized;
extern float sensorTemp, sensorHum;
extern int sensorLight, sensorAQI, sensorTVOC, sensorCO2;
extern SemaphoreHandle_t sensorMutex;

void parseLoRaMessage(const char *data);

void initializeLoRa() {
  LoRa.setPins(CS_PIN, RESET_PIN, IRQ_PIN);
  for (int attempt = 0; attempt < 5; attempt++) {
    Serial.printf("[LoRa] Attempt %d\n", attempt + 1);
    if (LoRa.begin(LORA_FREQ)) {
      loraInitialized = true;
      LoRa.setSpreadingFactor(7);
      LoRa.setSignalBandwidth(125E3);
      LoRa.enableCrc();
      LoRa.disableInvertIQ();
      LoRa.receive();
      Serial.println(F("[LoRa] Initialized"));
      return;
    }
    delay(1000);
  }
  loraInitialized = false;
  Serial.println(F("[LoRa] Initialization failed"));
}

void processLoRaPacket(int size) {
  if (size <= 0 || size >= sizeof(packetBuffer)) return;
  packetSize = size;
  for (int i = 0; i < size; i++) packetBuffer[i] = (char)LoRa.read();
  packetBuffer[size] = '\0';
  Serial.printf("[LoRa] Received packet: %s\n", packetBuffer);
  parseLoRaMessage(packetBuffer);
}

void parseLoRaMessage(const char *data) {
  int ti, td, hi, hd, l, a, tv, c;
  if (strncmp(data, "S_", 2) == 0 && sscanf(data, "S_%d.%d,%d.%d,%d,%d,%d,%d", &ti, &td, &hi, &hd, &l, &a, &tv, &c) == 8) {
    if (xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
      sensorTemp = ti + td / 10.0;
      sensorHum = hi + hd / 10.0;
      sensorLight = l; sensorAQI = a; sensorTVOC = tv; sensorCO2 = c;
      xSemaphoreGive(sensorMutex);
    }
    countpacket++;
    Serial.printf("[LoRa] Temp: %.1f°C, Hum: %.1f%%, Light: %d, AQI: %d, TVOC: %d, CO2: %d, Packet: %d\n",
                 sensorTemp, sensorHum, sensorLight, sensorAQI, sensorTVOC, sensorCO2, countpacket);
  } else {
    Serial.println("[LoRa] Failed to parse sensor data");
  }
}

#endif
