#pragma once

#include "../protocol/telemetry_packet.hpp"

namespace ares::telemetry {

class TelemetryCollector {
 public:
  void accept(const TelemetryPacket& packet);
  const TelemetryPacket& latest() const;

 private:
  TelemetryPacket latest_{};
};

}  // namespace ares::telemetry
