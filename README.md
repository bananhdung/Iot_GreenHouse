# GreenHouse IoT Control System

An IoT solution for smart greenhouse monitoring and control, powered by an Arduino-based **Sensor Node** and an ESP32-based **Controller Node**. The system leverages LoRa for sensor data transmission, WiFi for cloud connectivity, and the **Blynk** platform for remote monitoring and control. It supports manual, automatic, and timer-based relay control, safety alerts, persistent configuration, and thread-safe data handling.

---

## 🌿 Features

- **LoRa Communication**  
  - Sensor Node collects environmental data (temperature, humidity, light, AQI, TVOC, CO₂) and transmits via LoRa SX1278 (433 MHz, SF7, BW 125 kHz, CRC).
  - Controller Node receives and processes LoRa packets for real-time monitoring.

- **WiFi Connectivity**  
  - Connects to WiFi in Station mode or starts an Access Point (SSID: `GreenHouse`, Password: `12345678`) for configuration.
  - Web interface for runtime WiFi setup and system monitoring.

- **Blynk Integration**  
  - Remote monitoring and control via the Blynk mobile app.
  - Virtual pins for sensor data (V0–V5), relay control (V20–V25), mode switching (V13), and safety alerts (V15).

- **Environmental Monitoring**  
  - **AHTX0**: Temperature and humidity.
  - **TEMT6000**: Light intensity.
  - **ENS160**: Air Quality Index (AQI), TVOC, CO₂.

- **Relay Control**  
  - Manages 6 relays (air conditioner, fan, humidifier, lighting, air purifier, CO₂ system) via PCF8574 I²C.
  - **Modes**:  
    - **Manual**: Controlled via Blynk or web interface, locked at 23:00.
    - **Auto**: Based on sensor thresholds (e.g., TVOC > 100 ppb activates air purifier).
    - **Timer**: Scheduled activation via web interface.
    - **Night Lighting**: Activates lighting from 18:00 to 06:00 (up to 4 hours).

- **Cooling Fan**  
  - 12V fan on GPIO25, runs in 3-hour on, 30-minute off cycles to cool relays and ESP32.

- **Safety Alerts**  
  - Monitors sensor thresholds and triggers buzzer alerts for violations.
  - Sends safety messages to Blynk (e.g., "High temperature: 39°C > 38°C").
  - Buzzer signals LoRa failures (500ms on, 200ms off, 3 cycles).

- **Persistent Storage**  
  - WiFi credentials and settings saved using Non-Volatile Storage (NVS).

- **Thread Safety**  
  - Uses `sensorMutex` semaphore to manage shared sensor data across tasks.

---

## 🛠️ Hardware Requirements

### Sensor Node
- **Arduino Nano** (or compatible board).
- **Sensors**:
  - AHTX0 (Temperature & Humidity, I²C).
  - TEMT6000 (Light, Analog).
  - ENS160 (AQI, TVOC, CO₂, I²C).
- **LoRa SX1278 Module** (SPI: SS=D10, RST=D9, DIO0=D2).
- **Buzzer** (D7, for transmission failure alerts).

### Controller Node
- **ESP32 Development Board**.
- **LoRa SX1278 Module** (SPI: SCK=GPIO18, MISO=GPIO19, MOSI=GPIO27, CS=GPIO5, RST=GPIO14, IRQ=GPIO2).
- **PCF8574 I²C Expander** (for 8-channel relay module).
- **8-Channel Relay Module** (controls air conditioner, fan, humidifier, lighting, air purifier, CO₂ system).
- **12V Fan** (GPIO25).
- **Buzzer** (for safety and LoRa failure alerts).

---

## 💻 Software Requirements

- **Development Environment**:
  - Arduino IDE or PlatformIO.

- **Libraries**:
  - `BlynkSimpleEsp32` (Blynk connectivity).
  - `WiFi` (ESP32 WiFi management).
  - `Preferences` (NVS storage).
  - `ArduinoJson` (JSON for web server).
  - `LoRa` (LoRa SX1278 communication).
  - `Wire` (I²C communication).
  - `Adafruit_AHTX0` (AHTX0 sensor).
  - `Adafruit_ENS160` (ENS160 sensor).

- Install libraries via Arduino Library Manager or `platformio.ini`.

---

## 📱 Blynk App Setup

- **Template ID**: `TMPL6x9z1zKQW`
- **Virtual Pins**:
  - `V0`: Temperature (°C).
  - `V1`: Humidity (%).
    - `V2`: Light (lux).
  - `V3`: AQI.
  - `V4`: TVOC (ppb).
    - `V5`: CO₂ (ppm).
  - `V12`: Safety status (isSafe).
  - `V13`: Mode (0: Auto, 1: Manual).
  - `V15`: Safety message (alerts and recommendations).
  - `V15–V25`: Relay control (0: Air Conditioner, 1: Fan, 2: Humidifier, 3: Lighting, 4: Air Purifier, 5: CO₂ System).

- Configure the Blynky app with these pins to monitor sensors, control relays, and receive alerts.

---

## 🔧 Installation Instructions

1. **Install Libraries**:
   - Open Arduino IDE or PlatformIO.
   - Install the listed libraries using Library Manager or `platformio.ini`.

2. **Configure Blynk**:
   - Edit `blynk.h` to include your Blynk auth token:
     ```cpp
     #define BLYNK_AUTH_TOKEN "YourAuthTokenHere"
     ```

3. **WiFi Configuration**:
   - **Option 1**: Pre-configure in `config.h`:
     ```cpp
     const char* wifiSSID = "YourWiFiSSID";
     const char* wifiPassword = "YourWiFiPassword";
     ```
   - **Option 2**: Use Access Point mode:
     - Connect to `GreenHouse` (Password: `12345678`).
     - Access `http://192.168.4.1` to configure WiFi.

4. **Upload Code**:
   - **Sensor Node**:
     - Open `sensor.ino` in Arduino IDE.
     - **Compile and upload to Arduino Nano**.
   - **Controller Node**:
     - Open `main.ino` in Arduino IDE or PlatformIO.
     - Compile and upload to ESP32.

5. **Monitor and Control**:
   - Use Serial Monitor (115200 baud) to check WiFi, LoRa, and Blynk status.
   - Monitor sensor data and control relays via Blynk app.
   - Access the web interface (`http://<ESP32_IP>`) for local control.

---

## 📂 File Structure

### Sensor Node
- `sensor.ino`: Main firmware for sensor data collection and LoRa transmission.
- `sensors.h`: Sensor reading and calibration functions.
- `loRaRacomms.h`: LoRa SX1278 Communication logic.
- `config.h`: Sensor configuration settings.

### Controller Node
- `main.ino`: Hardware initialization and task management.
- `blynk.h`: Blynk connection and virtual pin handling.
- `wifi_manager.h`: WiFi and AP mode management.
- `lora_manager.h`: LoRa reception and parsing.
- `relay_control.h`: Relay control, timers, and automation logic.
- `fan_control.h`: Cooling fan control (GPIO25).
- `web_server.h`: Local web server and API endpoints.
- `config.h`: Global constants and configurations.

---

## 🚀 Usage

- **Access Point Mode**:
  - Activates if WiFi credentials are missing or invalid.
  - Connect to `GreenHouse` (Password: `12345678`) and configure via web interface at `http://192.168.4.1`.

- **Operating Modes**:
  - **Manual**: Control relays via Blynk (V20–V25) or web (`/setrelay`). Locked at 23:00.
  - **Auto**: Relays activate based on thresholds (e.g., temperature > 38°C triggers fan).
  - **Timer**: Schedule relays via web (`/settimer`).
  - **Night Lighting**: Lighting relay activates from 18:00 to 06:00 (up to 4 hours).

- **Safety Alerts**:
  - Threshold violations trigger buzzer and Blynk alerts (V15).
  - Example: "High temperature: 39°C > 38°C. Increase irrigation."

- **Cooling Fan**:
  - Runs in 3-hour on, 30-minute off cycles.

- **Serial Monitor**:
  - Logs WiFi, Blynk, LoRa, relay, and alert status.

---

## 🔍 Troubleshooting

- **WiFi Connection Issues**:
  - Verify `wifiSSID` and `wifiPassword` in `config.h`.
  - Use AP mode to reconfigure WiFi.
  - Check Serial Monitor for errors.

- **Blynk Connectivity Problems**:
  - Confirm `BLYNK_AUTH_TOKEN` in `blynk.h`.
  - Ensure internet access and Blynk server status.

- **LoRa Failures**:
  - Check LoRa module wiring (SPI pins) and 433 MHz frequency.
  - Serial Monitor logs initialization or packet errors.
  - Buzzer (500ms on, 200ms off, 3 cycles) signals LoRa failure.

- **Relay Issues**:
  - Ensure Manual Mode (V13 = 1) and `manualLocked = false`.
  - Verify PCF8574 I²C connection and relay power.

- **Missing Sensor Data**:
  - Check Sensor Node power and LoRa transmission.
  - Confirm Controller Node receives packets (Serial Monitor).

---

## 📜 License
MIT LicenseFree to use and modify for non-commercial purposes. See LICENSE for details.

## 📩 Contact
For questions, suggestions, or support:📧 bananhdung2003@gmail.comGitHub: bananhdung


