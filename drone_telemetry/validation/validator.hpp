#pragma once

#include "../protocol/telemetry_packet.hpp"

namespace ares::telemetry {

enum class ValidationStatus {
  Valid,
  InvalidTimestamp,
  InvalidSequence,
  NonFiniteValue,
  OutOfRange
};

class Validator {
 public:
  ValidationStatus validate(const TelemetryPacket& packet,
                            std::uint32_t previous_sequence,
                            std::int64_t now_ms) const;
};

}  // namespace ares::telemetry
