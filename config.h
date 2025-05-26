#ifndef CONFIG_H
#define CONFIG_H

// Pins
#define BUZZER_PIN      7
const long frequency = 433E6; 

const int csPin = 10;         
const int resetPin = 9;       
const int irqPin = 2;          
#define TEMT6000_PIN    A0

// Intervals
#define SENSOR_INTERVAL 1000  // Đọc cảm biến mỗi 1s
#define RESET_PACKET_COUNT 1000  // Reset sau 1000 gói
#define REST_DURATION   600000  // Nghỉ 10 phút (600,000 ms)

// Thresholds
#define CHANGE_PERCENT_TEMP  0.12f  // Nhiệt độ ≥12%
#define CHANGE_PERCENT_HUM   0.12f  // Độ ẩm ≥12%
#define CHANGE_PERCENT_LIGHT 0.30f  // Ánh sáng ≥30%
#define CHANGE_PERCENT_OTHER 0.25f  // AQI, TVOC, CO2 ≥25%

// Calibration factors
#define CALIBRATION_TEMP    0.9259  // Nhiệt độ: Giảm 12%
#define CALIBRATION_HUM     0.9259  // Độ ẩm: Giảm 12%
#define CALIBRATION_LIGHT   1.0000  // Ánh sáng: Không hiệu chỉnh
#define CALIBRATION_AQI     0.9709  // AQI: Giảm 3%
#define CALIBRATION_TVOC    0.9709  // TVOC: Giảm 3%
#define CALIBRATION_ECO2    0.9709  // CO2: Giảm 3%

#endif