#include "telemetry_collector.hpp"

namespace ares::telemetry {

void TelemetryCollector::accept(const TelemetryPacket& packet) {
  latest_ = packet;
}

const TelemetryPacket& TelemetryCollector::latest() const {
  return latest_;
}

}  // namespace ares::telemetry
