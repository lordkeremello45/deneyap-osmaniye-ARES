#pragma once
#include <cstdint>
#include <string>

namespace ares {
struct SensorFrame {
  double thermal_anomaly_c = 0.0;
  double uwb_distance_m = -1.0;
  double acoustic_rms = 0.0;
  double seismic_rms = 0.0;
  double lidar_distance_m = -1.0;
  std::int64_t timestamp_ms = 0;
};
class Engine {
 public:
  explicit Engine(std::string model_path);
  std::string analyze(const SensorFrame& frame) const;
 private:
  std::string model_path_;
};
}
