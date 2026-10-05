#pragma once

#include <cstdint>

namespace ares::telemetry {

struct ThermalData {
  float anomaly_c = 0.0F;
  std::uint16_t hot_pixel_count = 0;
};

struct AcousticData {
  float rms = 0.0F;
  float peak = 0.0F;
};

struct SeismicData {
  float rms = 0.0F;
  float peak = 0.0F;
};

struct TelemetryPacket {
  std::uint32_t sequence = 0;
  std::int64_t timestamp_ms = 0;
  ThermalData thermal{};
  float uwb_distance_m = -1.0F;
  AcousticData acoustic{};
  SeismicData seismic{};
  float lidar_distance_m = -1.0F;
};

}  // namespace ares::telemetry
