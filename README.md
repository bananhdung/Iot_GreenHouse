# GreenHouse IoT Control System

An ESP32-based IoT system for smart greenhouse monitoring and control using the Blynk app. This project supports environmental sensing, relay automation, WiFi configuration via access point mode, and persistent storage of settings.

---

## 🌿 Features

- **WiFi Connectivity**  
  Connects to existing WiFi or starts an access point (`GreenHouse`, password: `12345678`) for configuration.
  
- **Blynk Integration**  
  Remote monitoring and control through the Blynk app.

- **Environmental Sensor Monitoring**  
  - Temperature & Humidity: AHT2x  
  - Light: Temp6000  
  - Air Quality Index (AQI), TVOC, CO2: ENos16  

- **Relay Control**  
  - Controls up to 6 relays using PCF8574.
  - Manual and automatic modes.

- **Persistent Configuration**  
  WiFi credentials saved using Non-Volatile Storage (NVS).

- **Thread Safety**  
  Uses semaphores for managing sensor data across tasks.

---

## 🛠 Hardware Requirements
 
- **Adruino Nano Development Board**
- **ESP32 Development Board**
- **Sensors:**  
  - AHT2x (Temp & Humidity)  
  - Temp6000 (Light)  
  - ENos16 (AQI/TVOC/CO2)  
- **8-Channel Relay Module** (via PCF8574)  
- **Buzzer** (for alerts/notifications)  

---

## 💻 Software Requirements

- Arduino IDE **or** PlatformIO  
- Libraries:
  - `BlynkSimpleEsp32`
  - `WiFi`
  - `Preferences`
  - `ArduinoJson`

---

## 📲 Blynk App Setup

- **Template ID:** `TMPL6x9z1zKQW`
- **Virtual Pins:**
  - `V0 - V5`: Sensor Data (Temp, Humidity, Light, AQI, TVOC, CO2)
  - `V20 - V25`: Manual Relay Control
  - `V12, V13`: Mode Switching (Manual/Auto)

---

## 🔧 Setup Instructions

1. **Install Libraries**
   - Use Arduino Library Manager or PlatformIO to install required libraries.

2. **Configure Blynk**
   - In `blynk.h`, set your Blynk auth token:
     ```cpp
     #define BLYNK_AUTH_TOKEN "YourTokenHere"
     ```

3. **WiFi Configuration**
   - Edit `config.h` with:
     ```cpp
     const char* wifiSSID = "YourSSID";
     const char* wifiPassword = "YourPassword";
     ```
   - Or use AP mode (`GreenHouse` / `12345678`) for runtime configuration.

4. **Upload Code**
   - Open `main.ino` in Arduino IDE or PlatformIO.
   - Compile and upload to your ESP32 board.

5. **Connect & Monitor**
   - Check Serial Monitor (115200 baud) for WiFi and Blynk status.
   - Use the Blynk app to monitor sensor data and control relays.

---

## 📁 File Structure

- `main.ino` – Initializes hardware, manages tasks and logic.
- `sensor.ino` – Sensor reading and processing functions.
- `blynk.h` – Manages Blynk connection and virtual pin updates.
- `wifi_manager.h` – Handles WiFi & AP configuration.
- `config.h` – Global constants and persistent settings.
- `relay_control.h` – Relay management, timers, and logic.

---

## 🧪 Usage

- **AP Mode:**  
  Starts if WiFi config is missing or invalid.
  - SSID: `GreenHouse`  
  - Password: `12345678`

- **Modes:**
  - **Manual:** Use virtual pins `V20-V25` for relay control.
  - **Auto:** Relay state controlled via sensor thresholds.

- **Serial Monitor:**  
  Logs WiFi/Blynk status and relay activity.

---

## 🛠 Troubleshooting

- **WiFi Fails to Connect:**
  - Check `config.h` or re-enter via AP.
  - Monitor Serial output.

- **Blynk Connection Issues:**
  - Verify Blynk auth token and internet access.

- **Relay Not Responding:**
  - Ensure Manual Mode is active and `manualLocked` is false.

---

## 📜 License

MIT License  
Free to use and modify for **non-commercial purposes**.

---

## 📩 Contact

For questions, suggestions, or support, contact:  
📧 **bananhdung2003@gmail.com**

