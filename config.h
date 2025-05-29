#ifndef CONFIG_H
#define CONFIG_H

#include <freertos/semphr.h>

enum WiFiState { WIFI_IDLE, WIFI_CONNECTING, WIFI_CONNECTED, WIFI_ACCESS_POINT };

const long LORA_FREQ = 433E6;
const int CS_PIN = 5;
const int RESET_PIN = 14;
const int IRQ_PIN = 2;
const int FAN_RELAY_PIN = 25;
const uint8_t PCF8574_ADDRESS = 0x20;

#define RELAY_LIGHTING 0
#define RELAY_FAN 1
#define RELAY_HUMIDIFIER 2
#define RELAY_AIR_PURIFIER 3
#define RELAY_CO2_SYSTEM 4
#define RELAY_AIR 5

const unsigned long FAN_ON_TIME = 7200000UL;
const unsigned long FAN_OFF_TIME = 1800000UL;
const unsigned long RELAY_MIN_ON = 15 * 60 * 1000UL;
const unsigned long RELAY_MAX_ON = 2 * 3600 * 1000UL;
const unsigned long WIFI_TIMEOUT = 10000;

struct RelayTimer {
  int startHour;
  int startMinute;
  int endHour;
  int endMinute;
  bool enabled;
};

extern bool loraInitialized;
extern unsigned long fanLastChange;
extern bool fanOn;
extern float sensorTemp;
extern float sensorHum;
extern int sensorLight;
extern int sensorAQI;
extern int sensorTVOC;
extern int sensorCO2;
extern char packetBuffer[32];
extern int packetSize;
extern int countpacket;
extern SemaphoreHandle_t sensorMutex;
extern int mode;
extern bool relayStates[6];
extern unsigned long relayOnTimes[6];
extern unsigned long relayOffTimes[6];
extern RelayTimer relayTimers[6];
extern WiFiState wifiState;
extern unsigned long wifiConnectStart;
extern bool manualLocked;
extern String autoMessages[6];
extern String ssid;
extern String password;

#endif