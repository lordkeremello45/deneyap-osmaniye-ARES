#include "engine.hpp"
#include <iomanip>
#include <sstream>
#include <utility>
namespace ares {
Engine::Engine(std::string model_path) : model_path_(std::move(model_path)) {}
std::string Engine::analyze(const SensorFrame& f) const {
  const bool thermal = f.thermal_anomaly_c >= 2.0;
  const bool acoustic = f.acoustic_rms > 0.02;
  const bool seismic = f.seismic_rms > 0.01;
  const bool candidate = thermal && (acoustic || seismic);
  const double confidence = candidate ? 0.80 : (thermal ? 0.45 : 0.10);
  std::ostringstream out;
  out << std::fixed << std::setprecision(3)
      << "{\"version\":1,\"type\":\"human_presence\",\"status\":\""
      << (candidate ? "possible" : "no_signal") << "\",\"confidence\":" << confidence
      << ",\"thermal_anomaly_c\":" << f.thermal_anomaly_c
      << ",\"uwb_distance_m\":" << f.uwb_distance_m
      << ",\"acoustic_rms\":" << f.acoustic_rms
      << ",\"seismic_rms\":" << f.seismic_rms
      << ",\"lidar_distance_m\":" << f.lidar_distance_m
      << ",\"timestamp_ms\":" << f.timestamp_ms << "}";
  return out.str();
}
}
