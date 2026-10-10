#include <Arduino.h>
#include "sensor_drivers.h"
#if __has_include("ares_board_config.h")
#include "ares_board_config.h"
#endif
static const uint32_t kBaud=115200, kPeriodMs=1000;
namespace {
ares::sensors::Snapshot data;
HardwareSerial& telemetryPort() {
#if defined(ARES_LINK_RX_PIN) && defined(ARES_LINK_TX_PIN)
  return Serial2;
#else
  return Serial;
#endif
}
void field(const char* n,const ares::sensors::Reading& r) {
  telemetryPort().print('"'); telemetryPort().print(n); Serial.print(F("\":{\"status\":\""));
  Serial.print(ares::sensors::statusName(r.status)); Serial.print(F("\",\"valid\":"));
  Serial.print(r.valid?F("true"):F("false")); Serial.print(F(",\"timestamp_ms\":"));
  Serial.print(r.timestamp_ms); Serial.print(F(",\"value\":")); Serial.print(r.value); Serial.print('}');
}
}
void setup() {
  Serial.begin(kBaud); uint32_t start=millis();
  while(!Serial && uint32_t(millis()-start)<1500U) delay(10);
  ares::sensors::begin();
  telemetryPort().println(F("{\"type\":\"ares_firmware\",\"version\":1,\"state\":\"started\"}"));
}
void loop() {
  ares::sensors::poll(data); static uint32_t last=0; uint32_t now=millis();
  if(uint32_t(now-last)>=kPeriodMs) {
    last=now; Serial.print(F("{\"type\":\"sensor_status\",\"timestamp_ms\":")); Serial.print(now); Serial.print(',');
    field("lepton_tlinear_raw",data.lepton_tlinear_raw); Serial.print(',');
    field("lidar_mm",data.lidar_mm); Serial.print(',');
    field("geospace_adc_raw",data.geospace_adc_raw); Serial.print(',');
    field("dwm3000_mm",data.dwm3000_mm); Serial.print(',');
    field("xvf3800_audio_rms_raw",data.xvf3800_audio_rms_raw); Serial.println('}');
  }
  delay(2);
}
