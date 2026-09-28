# ARES architecture and roadmap

Goal: two equal Deneyap Kart V2 nodes, bidirectional UART, sensor acquisition, timestamps, validation, local logging and operator-facing status.

## Ordered plan
1. Define measurable requirements: payload mass, endurance, range, environment, cable length, baud rate, power budget and safety limits.
2. Research manufacturer datasheets, reference designs, supported drivers/licenses, voltage, interfaces, current, mass and availability.
3. Eliminate incompatible voltage/interface, undocumented, excessive-power/mass or legally unsafe candidates.
4. Bench-test remaining candidates; record measurements, firmware revision and setup.
5. Add advanced reliability: CRC, sequence numbers, ACK/timeouts/retries, sensor fusion, time sync, watchdog and fail-safe.
6. Build modular firmware, protocol tests, hardware acceptance tests and documentation.
7. Integrate sensors one at a time with regression tests.
8. Publish a static project site; live telemetry only after defining a safe data source.
9. Progress from bench tests to propellers-removed tests, then controlled permitted field trials.

## Software layout
firmware/common (framing, CRC, messages); firmware/node_a and node_b; firmware/drivers; tools (serial monitor/parser/replay); tests; docs; site.

## Open decisions
UART cable length/baud/physical layer; exact sensor models and power budget; flight-controller boundary; Flash access/use; site hosting and telemetry source.
