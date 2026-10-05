#include "../collector/telemetry_collector.hpp"
#include "../validation/validator.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>

int main() {
  using namespace ares::telemetry;

  TelemetryPacket packet{};
  packet.sequence = 10;
  packet.timestamp_ms = 100000;
  packet.thermal.anomaly_c = 3.2F;
  packet.acoustic.rms = 0.03F;
  packet.seismic.rms = 0.02F;
  packet.uwb_distance_m = 4.2F;
  packet.lidar_distance_m = 8.0F;

  Validator validator;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::Valid);

  packet.sequence = 9;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::InvalidSequence);

  packet.sequence = 11;
  packet.acoustic.rms = NAN;
  assert(validator.validate(packet, 9, 100001) == ValidationStatus::NonFiniteValue);

  return 0;
}
