#include "../collector/telemetry_collector.hpp"
#include "../validation/validator.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

int main() {
  using namespace ares::telemetry;
  TelemetryPacket packet{};
  packet.sequence = 10;
  packet.timestamp_ms = 100000;
  packet.thermal.anomaly_c = 3.2F;
  packet.acoustic.rms = 0.02F;
  packet.acoustic.peak = 0.03F;
  packet.seismic.rms = 0.01F;
  packet.seismic.peak = 0.02F;
  packet.uwb_distance_m = 4.2F;
  packet.lidar_distance_m = 8.0F;

  Validator validator;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::Valid);
  packet.sequence = 9;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::InvalidSequence);

  packet.sequence = 11;
  packet.acoustic.rms = std::numeric_limits<float>::quiet_NaN();
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::NonFiniteValue);

  packet.acoustic.rms = 0.02F;
  packet.acoustic.peak = -0.01F;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::OutOfRange);
  packet.acoustic.peak = 0.01F;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::OutOfRange);

  packet.acoustic.peak = 0.03F;
  packet.uwb_distance_m = -0.5F;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::OutOfRange);
  packet.uwb_distance_m = -1.0F;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::Valid);

  assert(validator.validate(packet, 9, 0) == ValidationStatus::InvalidTimestamp);
  assert(validator.validate(packet, 9, 106000) == ValidationStatus::InvalidTimestamp);

  // A monotonically increasing 32-bit counter remains valid across wrap-around.
  packet.sequence = 0U;
  assert(validator.validate(packet, 0xFFFFFFFFU, 100001) == ValidationStatus::Valid);

  TelemetryCollector collector;
  collector.accept(packet);
  assert(collector.latest().sequence == packet.sequence);
  return 0;
}
