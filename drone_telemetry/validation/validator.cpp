#include "validator.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace ares::telemetry {

ValidationStatus Validator::validate(const TelemetryPacket& packet,
                                     std::uint32_t previous_sequence,
                                     std::int64_t now_ms) const {
  if (packet.timestamp_ms <= 0 || now_ms <= 0 ||
      std::llabs(now_ms - packet.timestamp_ms) > 5000) {
    return ValidationStatus::InvalidTimestamp;
  }

  // RFC-style serial-number arithmetic permits uint32 wrap-around while
  // rejecting duplicates and values that are older by half the sequence space.
  const std::uint32_t sequence_delta = packet.sequence - previous_sequence;
  if (sequence_delta == 0U || sequence_delta > 0x7FFFFFFFU) {
    return ValidationStatus::InvalidSequence;
  }

  const float values[] = {
      packet.thermal.anomaly_c, packet.acoustic.rms, packet.acoustic.peak,
      packet.seismic.rms, packet.seismic.peak, packet.uwb_distance_m,
      packet.lidar_distance_m,
  };
  for (const float value : values) {
    if (!std::isfinite(value)) return ValidationStatus::NonFiniteValue;
  }

  // Generic transport bounds only; these are not sensor calibration limits.
  // Distance -1 is the unavailable sentinel.
  if (packet.thermal.anomaly_c < -100.0F ||
      packet.thermal.anomaly_c > 100.0F ||
      packet.acoustic.rms < 0.0F || packet.acoustic.peak < 0.0F ||
      packet.seismic.rms < 0.0F || packet.seismic.peak < 0.0F ||
      packet.acoustic.peak < packet.acoustic.rms ||
      packet.seismic.peak < packet.seismic.rms ||
      (packet.uwb_distance_m < 0.0F && packet.uwb_distance_m != -1.0F) ||
      (packet.lidar_distance_m < 0.0F && packet.lidar_distance_m != -1.0F)) {
    return ValidationStatus::OutOfRange;
  }
  return ValidationStatus::Valid;
}

}  // namespace ares::telemetry
