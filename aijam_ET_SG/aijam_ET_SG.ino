#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include "MAX30100_PulseOximeter.h"
#include "battery.h"
#include "BluetoothSerial.h"  // Bluetooth classic ESP32

// ===== CẢM BIẾN ADXL345 =====
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// ===== CẢM BIẾN MAX30100 =====
PulseOximeter pox;

// ===== BLUETOOTH SERIAL =====
BluetoothSerial SerialBT;

unsigned long lastPrintTime = 0;

// Callback khi phát hiện nhịp tim
void onBeatDetected() {
  Serial.println("💓 Beat detected!");
  SerialBT.println("💓 Beat detected!");
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_HealthBand"); // Tên thiết bị Bluetooth

  Serial.println("🔵 Bluetooth đã bật với tên: ESP32_HealthBand");
  SerialBT.println("🔵 ESP32 Bluetooth started");

  delay(100);

  // === Khởi tạo đo pin ===
  battery_begin();

  // === Khởi tạo ADXL345 ===
  if (!accel.begin()) {
    Serial.println("❌ ADXL345 không hoạt động.");
    SerialBT.println("❌ ADXL345 không hoạt động.");
  } else {
    accel.setRange(ADXL345_RANGE_4_G);
    Serial.println("✅ ADXL345 đã sẵn sàng.");
    SerialBT.println("✅ ADXL345 đã sẵn sàng.");
  }

  // === Khởi tạo MAX30100 ===
  if (!pox.begin()) {
    Serial.println("❌ MAX30100 không hoạt động.");
    SerialBT.println("❌ MAX30100 không hoạt động.");
  } else {
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
    Serial.println("✅ MAX30100 đã sẵn sàng.");
    SerialBT.println("✅ MAX30100 đã sẵn sàng.");
  }

  Serial.println("🚀 Hệ thống đã khởi động xong!");
  SerialBT.println("🚀 Hệ thống đã khởi động xong!");
}

void loop() {
  pox.update();  // Cập nhật dữ liệu cảm biến MAX30100

  if (millis() - lastPrintTime >= 1000) {
    lastPrintTime = millis();

    // === Đọc ADXL345 ===
    sensors_event_t event;
    accel.getEvent(&event);

    float x = event.acceleration.x;
    float y = event.acceleration.y;
    float z = event.acceleration.z;
    float totalG = sqrt(x * x + y * y + z * z);

    // === Đọc MAX30100 ===
    float bpm = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    // === Đọc pin ===
    float voltage = battery_get_voltage();
    int percent = battery_get_level();

    // === In ra UART và Bluetooth ===
    String message = "Total G: " + String(totalG, 2) +
                     " | BPM: " + String(bpm, 1) +
                     " | SpO2: " + String(spo2, 1) +
                     "% | Battery: " + String(percent) + "% (" + String(voltage, 2) + " V)";

    Serial.println(message);
    SerialBT.println(message);
  }
}
