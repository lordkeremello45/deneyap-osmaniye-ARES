#pragma once
#include <stdint.h>

namespace ares {
struct SensorPacket {
  uint16_t sequence;
  int16_t thermal_delta_c_x100;
  uint16_t uwb_distance_cm;
  uint16_t acoustic_rms_x1000;
  uint16_t seismic_rms_x1000;
  uint16_t lidar_distance_mm;
  uint32_t timestamp_ms;
};
}
