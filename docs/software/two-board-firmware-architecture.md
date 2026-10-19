# ARES two-board firmware architecture

**Status:** implementation scaffold; electrical pin mapping and hardware validation are pending.

## Responsibilities

| Node | Owns | Must not own |
|---|---|---|
| DENEYAP Card 2 — sensor node | FLIR Lepton 3.5, Garmin LIDAR-Lite v3, future calibrated GS-One analog front end, future DWM3000 driver, timestamp/status generation, optional SD logging | MQTT broker, Android pairing, AI inference, motor PWM or flight stabilization |
| DENEYAP Card 1 — gateway | bounded UART frame reception, JSON schema/version checks, Wi-Fi station, TLS MQTT telemetry publishing, diagnostic output | raw sensor electrical acquisition, unauthenticated remote commands, motor/flight control |
| Windows companion host | Go API, Mosquitto broker and credential lifecycle (planned), local AI/fusion, XVF3800 UAC2 host agent when the physical unit supports it | replacing the independent flight controller or treating AI output as confirmed survivor detection |
| Android operator app | operator display and authenticated pairing (planned) | storing fleet-wide shared secrets or issuing unbounded commands |

## Link and data contract

- Card 2 sends one UTF-8 JSON object per line at 115200 baud when the verified inter-board UART GPIOs are configured. TX/RX must be crossed and GND shared. Use short wiring and test EMI before raising the baud rate.
- Maximum accepted frame is 1024 bytes. Card 1 rejects overlong frames, invalid JSON, unexpected message types and missing timestamp_ms; it does not queue stale frames for later replay.
- Initial topic: ares/v1/telemetry/card2, QoS 0, retained=false. Card 1 is publish-only in this phase; no MQTT command subscriptions exist.
- Keep type, schema version, timestamp, status, validity and units explicit. Raw TLinear, raw ADC and uncalibrated acoustic features must never be represented as calibrated temperature, vibration, or survivor detection.
- millis() timestamps are local monotonic time and reset at boot; they are not UTC. Add a boot/session identifier and host receive timestamp before cross-device time correlation.

## Security controls introduced in this branch

- Go bridge binds to loopback only and refuses to start without a 32+ character ARES_BRIDGE_TOKEN.
- Audio/telemetry data APIs require Authorization: Bearer ...; token comparison is constant-time.
- Audio host agent accepts only numeric loopback destinations and attaches the bearer token, preventing its configurable URL from sending summaries to an external host.
- The gateway only enables MQTT if an untracked local secrets header explicitly enables it and provides Wi-Fi credentials, unique per-device MQTT credentials, broker host and a trusted root CA. TLS certificate validation is required; no insecure TLS bypass is present.
- MQTT client publishes only a validated, bounded telemetry frame to a fixed topic, with retained=false, and does not subscribe to commands.

## Build system and language interoperability

firmware/platformio.ini defines separate ares-card1-gateway and ares-card2-sensors environments. firmware/build_firmware.py is the build orchestrator: it invokes PlatformIO, collects each compiled binary and records SHA-256 metadata. It is not a source-language transpiler. The MCU runs compiled C/C++ machine code; Go and Python execute on the companion host. Versioned JSON is the shared data contract.

## Outstanding gates before operational use

1. Verify the physical DENEYAP Kart V2 revision/MCU and map board labels to raw GPIOs using the official core/pinout.
2. Assign collision-free inter-board UART pins and configure both ignored local headers; perform TX/RX/GND continuity checks.
3. Add a boot/session identifier, monotonic sequence counter, frame age policy and a UART corruption/overflow test.
4. Run PlatformIO builds and Go/Python tests in CI; record actual outputs rather than assuming success.
5. Configure Mosquitto TLS, anonymous-access denial, unique per-device credentials, topic ACLs, rotation/revocation and broker-side tests.
6. Integrate SD logging with bounded writes and recovery testing.
7. Finish DWM3000 as a separately validated ESP-IDF component and obtain a second compatible UWB node for ranging tests.
8. Calibrate each sensor against a known reference, then validate fusion with blinded test data. A sensor reading alone is not survivor confirmation.
9. Keep flight control and failsafe on a separate, validated flight controller.

## Failure behavior

- Missing board pin configuration: UART disabled, no attempt to infer GPIOs.
- Invalid/overlong UART frame: discard it and log a non-sensitive diagnostic.
- MQTT disconnected: drop current telemetry rather than replaying stale frames; continue local acquisition.
- Missing/invalid credentials or CA: MQTT build/connection remains disabled or fails closed.
- Sensor unavailable: emit explicit status/validity; never synthesize a range or detection.
