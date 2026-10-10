# ARES Software Status and Integration Gates

**Reviewed:** 2026-10-10  
**Scope:** Repository implementation status; hardware is tracked separately in [the hardware review report](../hardware/review-report-for-teacher.md).

## Implemented in this change set

- Telemetry validation rejects invalid timestamps, timestamps outside the configured 5-second window, non-finite values, negative RMS/peak, peak values lower than RMS, generic thermal bounds outside -100..100 °C anomaly, and invalid negative distance sentinels. These are generic transport checks, not sensor calibration.
- Telemetry unit tests cover valid frames, sequence rejection, NaN, negative/inconsistent peaks, unavailable distance sentinel, and timestamp bounds.
- AI Core rejects invalid timestamp, non-finite values and generic out-of-range input. Result contract explicitly returns confidence null, advisory_only true, and validated false. Status names avoid presenting a heuristic as confirmed human detection.
- AI Core has a CTest unit-test target for the result contract and invalid input cases.
- Go bridge binds only to a numeric loopback IP, requires a 32+ character bearer token for data APIs, applies request size limits, HTTP timeouts, graceful shutdown, JSON/no-store/security headers, content-type and method checks. The telemetry endpoint returns HTTP 503 with waiting_for_device and data null until a real telemetry adapter exists. It no longer returns a successful-looking placeholder as if the endpoint were operational.
- Go bridge unit tests cover health, unavailable telemetry, method rejection, bearer-token enforcement and content-type validation.

## Not yet implemented or verified

- No live UART/USB/Wi-Fi/MQTT telemetry adapter feeds the Go bridge or AI Core.
- The model-path argument does not load Gemma; no llama.cpp inference is implemented in AI Core.
- No calibrated sensor fusion or validated survivor detection exists. Thresholds are provisional and unvalidated.
- Firmware sensor acquisition remains a scaffold. GPIO assignments and module-specific integration are blocked on physical board/module verification.
- MQTT TLS, per-device credentials, ACL provisioning, pairing and secure remote API are design work, not operational features.
- Android controller is a prototype; bridge URL is now an explicit ARES_BRIDGE_URL build-time setting instead of a hardcoded localhost address. It defaults to unset, remote URLs require HTTPS, and the bridge itself binds only to loopback until authenticated remote access exists. Phone-to-host telemetry therefore remains intentionally unavailable until secure transport and a real telemetry adapter are implemented.
- No software change provides motor control or replaces an independent flight controller/failsafe.

## Required validation

Run ARES CI and inspect each job result before treating these changes as build-verified. Bench and hardware-in-loop tests remain necessary even after CI passes.


## Dual-board firmware work in progress

- PlatformIO now has separate Card 1 gateway and Card 2 sensor-node environments, plus a Python build wrapper that collects binaries and hashes. The build has **not yet been run** in this environment.
- Card 1 has a bounded newline-delimited UART frame parser, JSON type/timestamp checks and an optional TLS MQTT publisher guarded by local untracked configuration. It publishes telemetry only and subscribes to no commands.
- Card 2 can route sensor telemetry over UART2 only after verified raw GPIO numbers are configured; without those values it stays on USB serial. GPIO mapping is intentionally not guessed.
- MQTT broker provisioning, ACL tests, per-device credential rotation, physical UART tests and end-to-end TLS validation remain outstanding. Do not treat the scaffold as production-ready.
- The language/build tool is a PlatformIO build orchestrator, not a transpiler: C/C++ is compiled for the MCU; Go and Python execute on the companion host.
