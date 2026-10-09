#include "engine.hpp"

#include <cassert>
#include <limits>
#include <string>

int main() {
  ares::Engine engine("unused-in-current-baseline.gguf");
  ares::SensorFrame frame{};
  frame.timestamp_ms = 1000;
  frame.thermal_anomaly_c = 3.0;
  frame.acoustic_rms = 0.03;
  frame.seismic_rms = 0.0;
  frame.uwb_distance_m = -1.0;
  frame.lidar_distance_m = 2.0;

  const std::string candidate = engine.analyze(frame);
  assert(candidate.find("\"status\":\"possible_candidate\"") != std::string::npos);
  assert(candidate.find("\"confidence\":null") != std::string::npos);
  assert(candidate.find("\"advisory_only\":true") != std::string::npos);
  assert(candidate.find("\"validated\":false") != std::string::npos);

  frame.timestamp_ms = 0;
  const std::string invalid_timestamp = engine.analyze(frame);
  assert(invalid_timestamp.find("\"status\":\"invalid_input\"") != std::string::npos);
  assert(invalid_timestamp.find("invalid_timestamp") != std::string::npos);

  frame.timestamp_ms = 1001;
  frame.acoustic_rms = std::numeric_limits<double>::quiet_NaN();
  const std::string non_finite = engine.analyze(frame);
  assert(non_finite.find("non_finite_sensor_value") != std::string::npos);

  frame.acoustic_rms = 0.0;
  frame.uwb_distance_m = -0.5;
  const std::string out_of_range = engine.analyze(frame);
  assert(out_of_range.find("sensor_value_out_of_range") != std::string::npos);
  return 0;
}
