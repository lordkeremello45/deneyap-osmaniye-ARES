#include <Arduino.h>
static constexpr uint32_t UART_BAUD = 921600;
void setup() {
  Serial.begin(UART_BAUD);
  // Sensor drivers are isolated from transport and fusion.
  // Lock the exact Deneyap Kart V2 pinout before assigning sensor pins.
}
void loop() {
  // 1) sample sensors
  // 2) validate + timestamp
  // 3) frame SENSOR_DATA with CRC16
  // 4) publish to host
  delay(10);
}
