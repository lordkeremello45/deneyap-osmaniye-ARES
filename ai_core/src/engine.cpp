#include "engine.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>

namespace ares {
namespace {
bool finite_frame(const SensorFrame& frame) {
  return std::isfinite(frame.thermal_anomaly_c) &&
         std::isfinite(frame.uwb_distance_m) &&
         std::isfinite(frame.acoustic_rms) &&
         std::isfinite(frame.seismic_rms) &&
         std::isfinite(frame.lidar_distance_m);
}

std::string invalid_result(const char* reason) {
  std::ostringstream out;
  out << "{\"version\":1,\"type\":\"human_presence_advisory\","
      << "\"status\":\"invalid_input\",\"confidence\":null,"
      << "\"advisory_only\":true,\"validated\":false,"
      << "\"reason\":\"" << reason << "\"}";
  return out.str();
}
}  // namespace

Engine::Engine(std::string model_path) : model_path_(std::move(model_path)) {}

std::string Engine::analyze(const SensorFrame& frame) const {
  if (frame.timestamp_ms <= 0) return invalid_result("invalid_timestamp");
  if (!finite_frame(frame)) return invalid_result("non_finite_sensor_value");
  if (frame.thermal_anomaly_c < -100.0 || frame.thermal_anomaly_c > 100.0 ||
      frame.acoustic_rms < 0.0 || frame.seismic_rms < 0.0 ||
      (frame.uwb_distance_m < 0.0 && frame.uwb_distance_m != -1.0) ||
      (frame.lidar_distance_m < 0.0 && frame.lidar_distance_m != -1.0)) {
    return invalid_result("sensor_value_out_of_range");
  }

  // Transparent, uncalibrated baseline only—not model inference or a validated
  // survivor detector. Thresholds require controlled experimental validation.
  const bool thermal = frame.thermal_anomaly_c >= 2.0;
  const bool acoustic = frame.acoustic_rms > 0.02;
  const bool seismic = frame.seismic_rms > 0.01;
  const bool candidate = thermal && (acoustic || seismic);

  std::ostringstream out;
  out << std::fixed << std::setprecision(3)
      << "{\"version\":1,\"type\":\"human_presence_advisory\",\"status\":\""
      << (candidate ? "possible_candidate" : "no_correlated_signal")
      << "\",\"confidence\":null,\"advisory_only\":true,"
      << "\"validated\":false,\"method\":\"uncalibrated_threshold_baseline\""
      << ",\"thermal_anomaly_c\":" << frame.thermal_anomaly_c
      << ",\"uwb_distance_m\":" << frame.uwb_distance_m
      << ",\"acoustic_rms\":" << frame.acoustic_rms
      << ",\"seismic_rms\":" << frame.seismic_rms
      << ",\"lidar_distance_m\":" << frame.lidar_distance_m
      << ",\"timestamp_ms\":" << frame.timestamp_ms << "}";
  return out.str();
}
}  // namespace ares
