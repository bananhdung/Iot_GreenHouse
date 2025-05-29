#ifndef WEB_H
#define WEB_H


const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>IoT - Đại học Hạ Long</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f0f0f0;
      color: #333;
    }

    /* Banner Styles */
    .banner {
      width: 100%;
      height: 100px;
      background-color: #2c3e50;
      color: white;
      display: flex;
      align-items: center;
      overflow: hidden;
    }

    .logo {
      width: 80px;
      height: 80px;
      border-radius: 50%;
      margin-left: 20px;
      flex-shrink: 0;
      z-index: 1;
    }

    .carousel-container {
      flex: 1;
      overflow: hidden;
      position: relative;
    }

    .carousel {
      display: flex;
      width: fit-content;
      animation: scroll-right-to-left 15s linear infinite;
    }

    .slide {
      display: flex;
      align-items: center;
      padding-right: 50px;
      font-size: 20px;
      white-space: nowrap;
    }

    @keyframes scroll-right-to-left {
      0% { transform: translateX(0); }
      100% { transform: translateX(-50%); }
    }

    /* Container and Section */
    .container {
      max-width: 1200px;
      margin: 20px auto;
      padding: 0 20px;
    }

    .section {
      background: #fff;
      padding: 20px;
      margin-bottom: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
    }

    h1, h2, h3 {
      color: #2c3e50;
      text-align: center;
    }

    /* Status Bar */
    .status-bar {
      background-color: #3498db;
      color: white;
      padding: 10px;
      text-align: center;
      animation: blink 1.5s infinite;
      border-radius: 5px;
      margin-bottom: 20px;
    }

    @keyframes blink {
      0% { opacity: 1; }
      50% { opacity: 0.5; }
      100% { opacity: 1; }
    }

    /* Sensor Data */
    .sensor-data {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 20px;
    }

    .sensor-item {
      text-align: center;
    }

    /* Relay Controls */
    .relay-controls {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 10px;
    }

    .relay-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px;
      background-color: #f9f9f9;
      border-radius: 5px;
    }

    .relay-item label.mode-label {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    /* Timer Form */
    .timer-form {
      display: grid;
      grid-template-columns: 150px 100px 100px 60px 80px;
      align-items: center;
      gap: 10px;
      margin-bottom: 10px;
    }

    .timer-form button {
      padding: 5px 10px;
      background-color: #2ecc71;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
    }

    /* Relay Timer Container */
    .relay-timer-container {
      display: grid;
      grid-template-columns: 1fr;
      gap: 20px;
    }

    @media (min-width: 768px) {
      .relay-timer-container {
        grid-template-columns: 1fr 1fr;
      }
    }

    /* Switch Styles */
    .switch {
      position: relative;
      display: inline-block;
      width: 50px;
      height: 24px;
    }

    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
      border-radius: 24px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 18px;
      width: 18px;
      left: 3px;
      bottom: 3px;
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }

    .switch input:checked + .slider {
      background-color: #2ecc71;
    }

    .switch input:checked + .slider:before {
      transform: translateX(26px);
    }

    .switch.disabled .slider {
      background-color: #ddd;
      cursor: not-allowed;
    }

    /* WiFi Section */
    .wifi-section {
      display: none;
      flex-direction: column;
      gap: 20px;
    }

    @media (min-width: 768px) {
      .wifi-section {
        flex-direction: row;
      }
      .wifi-section .section {
        width: 48%;
      }
    }

    .wifi-icon {
      font-size: 24px;
      cursor: pointer;
      text-align: center;
      margin-bottom: 15px;
      color: #2c3e50;
    }

    .wifi-icon:hover {
      color: #3498db;
    }

    .wifi-form {
      display: grid;
      grid-template-columns: 80px 1fr;
      gap: 10px;
      align-items: center;
      margin-bottom: 15px;
    }

    .wifi-form input[type="text"],
    .wifi-form input[type="password"] {
      padding: 8px;
      border: 1px solid #ccc;
      border-radius: 5px;
      width: 200px; /* Đặt chiều rộng cố định để hai textbox bằng nhau */
      box-sizing: border-box; /* Đảm bảo padding không làm tăng kích thước */
    }

    .password-container {
      position: relative;
      display: flex;
      align-items: center;
      width: 200px; /* Đảm bảo container của password cũng có cùng chiều rộng */
    }

    .password-container input {
      width: 100%;
      padding-right: 30px;
    }

    .password-container .eye-icon {
      position: absolute;
      right: 10px;
      cursor: pointer;
      color: #666;
    }

    .wifi-buttons {
      display: flex;
      justify-content: space-between;
      gap: 10px;
    }

    .wifi-buttons button {
      flex: 1;
      padding: 10px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      font-size: 16px;
      color: white;
    }

    .wifi-buttons button:first-child {
      background-color: #3498db;
    }

    .wifi-buttons button:last-child {
      background-color: #2ecc71;
    }

    .networks-list p {
      cursor: pointer;
      padding: 10px;
      margin: 5px 0;
      background-color: #f9f9f9;
      border-radius: 5px;
    }

    .networks-list p:hover {
      background-color: #e0e0e0;
    }

    /* Alert Box */
    .alert {
      display: none;
      position: fixed;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      background-color: #f44336;
      color: white;
      padding: 15px;
      border-radius: 5px;
      z-index: 1000;
    }
    /* Warning Section */
    .warning-section {
      margin-top: 20px;
    }

    .warning-item {
      padding: 10px;
      margin-bottom: 10px;
      border-radius: 5px;
      background-color: #fff3cd;
      color: #856404;
    }

    .warning-item.critical {
      background-color: #f8d7da;
      color: #721c24;
    }

    .warning-item p {
      margin: 0;
    }
  </style>
</head>
<body>
  <!-- Banner -->
  <div class="banner">
    <img src="https://uhl.edu.vn/Content/assets/images/header/logo_halong.jpg" alt="Logo Đại học Hạ Long" class="logo">
    <div class="carousel-container">
      <div class="carousel">
        <div class="slide"><span><strong>Đại học Hạ Long</strong> - Hệ thống IoT giám sát và Điều khiển nhà kính thông minh</span></div>
        <div class="slide"><span>Tác giả: Ban Anh Dung|21DH03108</span></div>
        <div class="slide"><span><strong>Đại học Hạ Long</strong> - Hệ thống IoT giám sát và Điều khiển nhà kính thông minh</span></div>
        <div class="slide"><span>Tác giả: Ban Anh Dung|21DH03108</span></div>
      </div>
    </div>
  </div>

  <!-- Main Container -->
  <div class="container">
    <!-- Status Bar -->
    <div class="status-bar" id="statusBar">
      <i class="fas fa-wifi"></i> WiFi: <span id="wifiStatus">--</span> |
      <i class="fas fa-signal"></i> LoRa: <span id="loraStatus">--</span>
    </div>

    <!-- WiFi Configuration -->
    <div class="section">
      <div class="wifi-icon" id="wifiIcon">
        <i class="fas fa-wifi"></i> Cấu hình WiFi
      </div>
      <div class="wifi-section" id="wifiSection">
        <div class="section" id="wifiList">
          <h3>Danh sách WiFi</h3>
          <div class="networks-list" id="networks"></div>
        </div>
        <div class="section" id="wifiForm">
          <h3>Kết nối WiFi</h3>
          <div class="wifi-form">
            <label>SSID:</label>
            <input type="text" id="ssid">
            <label>Mật khẩu:</label>
            <div class="password-container">
              <input type="password" id="password">
              <i class="fas fa-eye eye-icon" id="eyeIcon" onclick="togglePassword()"></i>
            </div>
          </div>
          <div class="wifi-buttons">
            <button onclick="scanWiFi()">Quét</button>
            <button onclick="setWiFi()">Kết nối</button>
          </div>
        </div>
      </div>
    </div>

    <!-- Sensor Data -->
    <div class="section">
      <h3>Dữ liệu cảm biến</h3>
      <div class="sensor-data" id="sensorData">
        <div class="sensor-item"><div>🌡️</div><p>Temperature: <span id="temp">--</span>°C</p></div>
        <div class="sensor-item"><div>💧</div><p>Humidity: <span id="hum">--</span>%</p></div>
        <div class="sensor-item"><div>💡</div><p>Light: <span id="light">--</span> lux</p></div>
      </div>
      <div class="sensor-data" id="sensorData">
        <div class="sensor-item"><div>🌫️</div><p>AQI: <span id="aqi">--</span></p></div>
        <div class="sensor-item"><div>☣️</div><p>TVOC: <span id="tvoc">--</span> ppb</p></div>
        <div class="sensor-item"><div>🌬️</div><p>CO2: <span id="co2">--</span> ppm</p></div>
      </div>
    </div>

    <!-- Relay and Timer Controls -->
    <div class="section">
      <h3>Điều khiển & Hẹn giờ</h3>
      <div class="relay-timer-container">
        <div>
          <h4>Điều khiển Relay</h4>
          <div class="relay-controls" id="relayControls">
            <div class="relay-item">
              <label class="mode-label">Chế độ: <span id="modeText">Tự động</span></label>
              <label class="switch">
                <input type="checkbox" id="modeSwitch" onchange="setMode(this.checked)">
                <span class="slider"></span>
              </label>
            </div>
            <div class="relay-item">
              <label>Lighting</label>
              <label class="switch">
                <input type="checkbox" id="relay0" onchange="toggleRelay(0)">
                <span class="slider"></span>
              </label>
            </div>
            <div class="relay-item">
              <label>Fan</label>
              <label class="switch">
                <input type="checkbox" id="relay1" onchange="toggleRelay(1)">
                <span class="slider"></span>
              </label>
            </div>
            <div class="relay-item">
              <label>Humidifier</label>
              <label class="switch">
                <input type="checkbox" id="relay2" onchange="toggleRelay(2)">
                <span class="slider"></span>
              </label>
            </div>
            <div class="relay-item">
              <label>Air Purifier</label>
              <label class="switch">
                <input type="checkbox" id="relay3" onchange="toggleRelay(3)">
                <span class="slider"></span>
              </label>
            </div>
            <div class="relay-item">
              <label>CO2 System</label>
              <label class="switch">
                <input type="checkbox" id="relay4" onchange="toggleRelay(4)">
                <span class="slider"></span>
              </label>
            </div>
            <div class="relay-item">
              <label>Air</label>
              <label class="switch">
                <input type="checkbox" id="relay5" onchange="toggleRelay(5)">
                <span class="slider"></span>
              </label>
            </div>
          </div>
        </div>
        <div>
          <h4>Hẹn giờ</h4>
          <div id="timerSettings">
            <div class="timer-form">
              <label>Lighting:</label>
              <input type="time" id="start0">
              <input type="time" id="end0">
              <label class="switch">
                <input type="checkbox" id="enabled0">
                <span class="slider"></span>
              </label>
              <button onclick="setTimer(0)">Cài đặt</button>
            </div>
            <div class="timer-form">
              <label>Fan:</label>
              <input type="time" id="start1">
              <input type="time" id="end1">
              <label class="switch">
                <input type="checkbox" id="enabled1">
                <span class="slider"></span>
              </label>
              <button onclick="setTimer(1)">Cài đặt</button>
            </div>
            <div class="timer-form">
              <label>Humidifier:</label>
              <input type="time" id="start2">
              <input type="time" id="end2">
              <label class="switch">
                <input type="checkbox" id="enabled2">
                <span class="slider"></span>
              </label>
              <button onclick="setTimer(2)">Cài đặt</button>
            </div>
            <div class="timer-form">
              <label>Air Purifier:</label>
              <input type="time" id="start3">
              <input type="time" id="end3">
              <label class="switch">
                <input type="checkbox" id="enabled3">
                <span class="slider"></span>
              </label>
              <button onclick="setTimer(3)">Cài đặt</button>
            </div>
            <div class="timer-form">
              <label>CO2 System:</label>
              <input type="time" id="start4">
              <input type="time" id="end4">
              <label class="switch">
                <input type="checkbox" id="enabled4">
                <span class="slider"></span>
              </label>
              <button onclick="setTimer(4)">Cài đặt</button>
            </div>
            <div class="timer-form">
              <label>Air:</label>
              <input type="time" id="start5">
              <input type="time" id="end5">
              <label class="switch">
                <input type="checkbox" id="enabled5">
                <span class="slider"></span>
              </label>
              <button onclick="setTimer(5)">Cài đặt</button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- Alert Box -->
  <div class="alert" id="alertBox"></div>

  <script>
    let currentMode = 0; // 0: Auto, 1: Manual

    document.addEventListener('DOMContentLoaded', function() {
      document.getElementById('wifiIcon').addEventListener('click', toggleWiFiSection);
      setInterval(updateData, 2000);
      updateData();
    });

    function togglePassword() {
      var passwordInput = document.getElementById("password");
      var eyeIcon = document.getElementById("eyeIcon");
      if (passwordInput.type === "password") {
        passwordInput.type = "text";
        eyeIcon.classList.remove("fa-eye");
        eyeIcon.classList.add("fa-eye-slash");
      } else {
        passwordInput.type = "password";
        eyeIcon.classList.remove("fa-eye-slash");
        eyeIcon.classList.add("fa-eye");
      }
    }

    function toggleWiFiSection() {
      var wifiSection = document.getElementById("wifiSection");
      if (wifiSection.style.display === "none" || wifiSection.style.display === "") {
        wifiSection.style.display = "flex";
        scanWiFi();
      } else {
        wifiSection.style.display = "none";
      }
    }

    function scanWiFi() {
      fetch('/scanwifi')
        .then(response => response.json())
        .then(data => {
          var networksDiv = document.getElementById("networks");
          networksDiv.innerHTML = data.map(net => `<p onclick="selectWiFi('${net.ssid}')">${net.ssid} (${net.rssi})</p>`).join('');
        });
    }

    function selectWiFi(ssid) {
      document.getElementById("ssid").value = ssid;
    }

    function setWiFi() {
        var ssid = document.getElementById("ssid").value;
        var password = document.getElementById("password").value;
        fetch('/setwifi?ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password))
            .then(response => response.text())
            .then(data => {
                alert(data); // Show response message
                // Reset textboxes
                document.getElementById("ssid").value = "";
                document.getElementById("password").value = "";
                // Reload the page
                window.location.reload();
            })
            .catch(error => {
                console.error('Error:', error);
                alert('Failed to set WiFi');
            });
    }

    function setMode(isManual) {
      var newMode = isManual ? 1 : 0;
      if (currentMode === 1 && isManual) {
        newMode = 0;
        document.getElementById('modeSwitch').checked = false;
        showAlert('Đã chuyển về chế độ tự động!');
      }
      fetch('/setmode?mode=' + newMode)
        .then(response => response.text())
        .then(data => {
          currentMode = newMode;
          document.getElementById('modeText').textContent = currentMode ? 'Thủ công' : 'Tự động';
          updateRelaySwitches();
        });
    }

    function updateRelaySwitches() {
      var switches = document.querySelectorAll("#relayControls input[type='checkbox']:not(#modeSwitch)");
      switches.forEach(function(switchInput) {
        if (currentMode === 0) {
          switchInput.disabled = true;
          switchInput.parentElement.classList.add('disabled');
        } else {
          switchInput.disabled = false;
          switchInput.parentElement.classList.remove('disabled');
        }
      });
    }

    function toggleRelay(relay) {
      if (currentMode === 0) {
        showAlert('Đang ở chế độ tự động, không thể điều khiển thủ công!');
        return;
      }
      var relayInput = document.getElementById("relay" + relay);
      fetch('/setrelay?relay=' + relay + '&state=' + (relayInput.checked ? 1 : 0))
        .then(response => response.text())
        .then(data => console.log('Relay ' + relay + ' set to', relayInput.checked));
    }

    function setTimer(relay) {
      var start = document.getElementById("start" + relay).value;
      var end = document.getElementById("end" + relay).value;
      var enabled = document.getElementById("enabled" + relay).checked;
      fetch('/settimer?relay=' + relay + '&start=' + start + '&end=' + end + '&enabled=' + (enabled ? 1 : 0))
        .then(response => response.text())
        .then(data => console.log('Timer for relay ' + relay + ' set'));
    }

    function showAlert(message) {
      var alertBox = document.getElementById("alertBox");
      alertBox.textContent = message;
      alertBox.style.display = "block";
      setTimeout(function() {
        alertBox.style.display = "none";
      }, 3000);
    }

    function updateData() {
      fetch('/sensors')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').textContent = data.temp;
          document.getElementById('hum').textContent = data.hum;
          document.getElementById('light').textContent = data.light;
          document.getElementById('aqi').textContent = data.aqi;
          document.getElementById('tvoc').textContent = data.tvoc;
          document.getElementById('co2').textContent = data.co2;
        });
      fetch('/relays')
        .then(response => response.json())
        .then(data => {
          for (var i = 0; i < 6; i++) {
            document.getElementById('relay' + i).checked = data[i] == 1;
          }
        });
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('wifiStatus').textContent = data.wifi;
          document.getElementById('loraStatus').textContent = data.lora;
          currentMode = data.mode;
          document.getElementById('modeSwitch').checked = currentMode === 1;
          document.getElementById('modeText').textContent = currentMode ? 'Thủ công' : 'Tự động';
          updateRelaySwitches();
        });
    }
  </script>
</body>
</html>
)rawliteral";

#endif