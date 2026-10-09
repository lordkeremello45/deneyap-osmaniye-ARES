#pragma once
#include <Arduino.h>
#include <stdint.h>
namespace ares::sensors {
enum class Status : uint8_t { Disabled, Ready, SampleReady, NoFrame, ReadError, NotIntegrated, InvalidData };
struct Reading { Status status{Status::Disabled}; uint32_t timestamp_ms{0}; int32_t value{0}; bool valid{false}; };
struct Snapshot { Reading lepton_tlinear_raw; Reading lidar_mm; Reading geospace_adc_raw; Reading dwm3000_mm; Reading xvf3800_audio_rms_raw; };
void begin();
void poll(Snapshot& out);
const char* statusName(Status status);
}  // namespace ares::sensors
