# ARES Mobile — planned phase

Flutter application for the future Android/iOS operator interface. End-to-end mobile pairing and MQTT provisioning are **not implemented yet**.

## Intended connection flow

1. Windows Ground Station starts/validates the local Mosquitto service and pairing endpoint.
2. The operator pairs the phone using a six-digit, single-use PIN shown by Windows. The PIN expires after 120 seconds and failed attempts are rate-limited.
3. After successful verification over authenticated TLS, Windows creates a unique MQTT identity and random secret for this phone, then grants least-privilege topic ACLs.
4. The app stores the secret in platform secure storage (Android Keystore-backed storage on Android); it must never be logged or committed to source control.
5. The app connects using TLS and the per-phone credential. Revoked credentials must stop working; re-pairing creates a new identity.

The six-digit PIN is only a short-lived bootstrap code, not the MQTT secret. Do not accept arbitrary TLS certificates or disable certificate validation. See [MQTT pairing and provisioning security specification](../docs/security/mqtt-pairing.md).

## Responsibilities

- Flutter/Dart: operator UI, pairing screens, secure credential storage integration, connection state, and command confirmation.
- Windows Ground Station: local broker lifecycle, pairing authority, credential generation/revocation, topic ACLs, telemetry ingestion, AI Core integration, and command validation.
- Mosquitto: authenticated local MQTT transport; no anonymous access or public Internet listener by default.
- ESP32 nodes: separate device identities only after a safe provisioning method and supported secret storage are validated. Never share the phone's credential with a drone node.

MQTT carries telemetry metadata, sensor summaries, AI results, and command envelopes—not unbounded raw audio or thermal streams. Commands require freshness/expiry, unique IDs, replay suppression, schema checks, authorization, and device-state validation. Flight stabilization, motor PWM, and onboard failsafe must remain independent of Windows, MQTT, and Gemma.

This is a target design, not a claim that TLS, pairing, Mosquitto installation, MQTT ACLs, or end-to-end mobile integration currently work.
