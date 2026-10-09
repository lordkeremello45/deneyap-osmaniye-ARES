#pragma once
#include <Arduino.h>
#include <stdint.h>
namespace ares { namespace sensors {
enum class Status : uint8_t { Disabled, SampleReady, NoFrame, ReadError, NotIntegrated, InvalidData };
struct Reading {
  Status status; uint32_t timestamp_ms; int32_t value; bool valid;
  Reading(Status s = Status::Disabled, uint32_t t = 0, int32_t v = 0, bool ok = false) : status(s), timestamp_ms(t), value(v), valid(ok) {}
};
struct Snapshot { Reading lepton_tlinear_raw; Reading lidar_mm; Reading geospace_adc_raw; Reading dwm3000_mm; Reading xvf3800_audio_rms_raw; };
void begin(); void poll(Snapshot& out); const char* statusName(Status status);
} }
