#include <Arduino.h>
#include "sensor_drivers.h"
#if __has_include("ares_board_config.h")
#include "ares_board_config.h"
#endif

static const uint32_t kBaud = 115200;
static const uint32_t kPeriodMs = 1000;

namespace {
ares::sensors::Snapshot data;

HardwareSerial& telemetryPort() {
#if defined(ARES_LINK_RX_PIN) && defined(ARES_LINK_TX_PIN)
  return Serial2;
#else
  return Serial;
#endif
}

void field(const char* name, const ares::sensors::Reading& reading) {
  HardwareSerial& out = telemetryPort();
  out.print('"');
  out.print(name);
  out.print(F("\":{\"status\":\""));
  out.print(ares::sensors::statusName(reading.status));
  out.print(F("\",\"valid\":"));
  out.print(reading.valid ? F("true") : F("false"));
  out.print(F(",\"timestamp_ms\":"));
  out.print(reading.timestamp_ms);
  out.print(F(",\"value\":"));
  out.print(reading.value);
  out.print('}');
}
}

void setup() {
  Serial.begin(kBaud);
#if defined(ARES_LINK_RX_PIN) && defined(ARES_LINK_TX_PIN)
  // Raw GPIO numbers must be verified for the exact physical board revision.
  Serial2.begin(kBaud, SERIAL_8N1, ARES_LINK_RX_PIN, ARES_LINK_TX_PIN);
#endif
  const uint32_t start = millis();
  while (!Serial && uint32_t(millis() - start) < 1500U) delay(10);
  ares::sensors::begin();
  telemetryPort().println(F("{\"type\":\"ares_firmware\",\"version\":1,\"state\":\"started\"}"));
}

void loop() {
  ares::sensors::poll(data);
  static uint32_t last = 0;
  const uint32_t now = millis();
  if (uint32_t(now - last) >= kPeriodMs) {
    last = now;
    HardwareSerial& out = telemetryPort();
    out.print(F("{\"type\":\"sensor_status\",\"timestamp_ms\":"));
    out.print(now);
    out.print(',');
    field("lepton_tlinear_raw", data.lepton_tlinear_raw); out.print(',');
    field("lidar_mm", data.lidar_mm); out.print(',');
    field("geospace_adc_raw", data.geospace_adc_raw); out.print(',');
    field("dwm3000_mm", data.dwm3000_mm); out.print(',');
    field("xvf3800_audio_rms_raw", data.xvf3800_audio_rms_raw);
    out.println('}');
  }
  delay(2);
}
