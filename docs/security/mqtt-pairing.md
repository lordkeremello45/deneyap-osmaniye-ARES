# ARES MQTT provisioning and device pairing

**Status: target security design; not implemented or security-tested yet.** This document defines intended behavior before transport code is added.

## Decision

- Windows Ground Station owns the local Mosquitto broker, pairing authority, device registry, and credential lifecycle.
- The Windows installer may offer to install Mosquitto automatically when absent, then configure and start it as a Windows service. Installation must be explicit/visible, version-pinned or verified, and fail closed if configuration or health checks fail. Do not silently expose an unauthenticated broker.
- Phone-to-Windows pairing uses a short-lived six-digit one-time PIN. After successful verification, Windows provisions a **unique per-device MQTT username and random secret** automatically. The PIN is only a bootstrap proof, not the MQTT key.
- Every client gets its own credential and topic ACL. Never reuse a single shared key across the phone, ESP32 nodes, and other clients.
- The broker listens only on the intended trusted LAN interface by default, with authentication and TLS enabled. No public Internet listener or unauthenticated anonymous access.

## Pairing flow

1. Windows Ground Station starts its local pairing service and checks Mosquitto health/configuration.
2. Windows generates a cryptographically random six-digit PIN using the OS CSPRNG, shows it to the operator, and binds it to one pairing session.
3. The phone connects to the Windows pairing endpoint over authenticated TLS and submits the PIN. The operator must verify the Windows identity/certificate shown by the app; do not accept arbitrary certificates.
4. The server verifies the PIN in constant-time where practical. PIN expires after **120 seconds**, is single-use, and allows at most **5 failed attempts per session** before invalidation/cooldown. Apply additional per-client/IP throttling and avoid logging PINs.
5. After successful pairing, Windows creates a new device identity and unique high-entropy MQTT secret, applies a least-privilege ACL, and returns the credential once over the authenticated TLS channel.
6. The phone stores the secret in Android Keystore-backed secure storage (or the platform equivalent); do not write it to source files, logs, plain preferences, or exported backups.
7. Revocation immediately disables that identity. Re-pairing creates a new credential; it does not reveal or restore the old secret.

### ESP32/Deneyap node provisioning

Provision each board as a separate identity. Do not copy the phone credential or use a fleet-wide secret. The initial enrollment transport and secure storage must be selected after validating the exact Deneyap Kart V2/ESP32 variant and its supported secure-boot/flash-encryption configuration. Until that is tested, do not claim that secrets are protected against physical extraction. Do not send a permanent credential over plaintext UART or an open Wi-Fi network.

## MQTT broker and ACL policy

- Use a unique username and random secret per client (minimum 256 bits of generated secret material; encode for transport/storage as needed).
- Configure Mosquitto authentication and per-client topic ACLs through a supported management mechanism, such as its Dynamic Security plugin where available and validated. If that mechanism is not installed/configured, provisioning must fail rather than fall back to anonymous access.
- Example topic separation:
  - ares/v1/telemetry/<device_id>/... — device publishes its own telemetry only.
  - ares/v1/status/<device_id>/... — device publishes its own status only.
  - ares/v1/command/<device_id>/... — authorized Ground Station publishes commands; a device may subscribe only to its own command topic.
  - ares/v1/pairing/... — pairing service only; never expose MQTT credentials on general telemetry topics.
- Reject retained command messages. Commands require schema/version checks, unique command IDs, expiry/deadline, replay/duplicate suppression, authorization, and explicit device-state validation.
- MQTT carries telemetry metadata, sensor summaries, AI results, and command envelopes. Bulk audio/thermal data should use a separately designed bounded transfer channel or files; do not flood the broker with raw high-rate streams.
- Keep flight stabilization, motor PWM, and onboard failsafe independent of MQTT, Windows, and Gemma. On link loss, the flight controller's validated failsafe policy applies.

## Secret storage and cryptographic boundaries

- **SPARK is not a key vault and does not itself encrypt or protect secrets.** Use maintained platform cryptography and operating-system secure storage. SPARK can specify/prove selected state-machine invariants (for example, a command is accepted only after authorization, freshness, and range checks), but must not be presented as proof of cryptographic correctness.
- Never commit real secrets, PINs, certificates with private keys, or generated credentials to Git. Keep configuration templates separate from local secrets.
- Prefer TLS 1.2+ with certificate validation; pin or securely enroll the local broker/service identity so a hostile LAN peer cannot impersonate Windows. Avoid disabling certificate verification to “make pairing work.”
- Keep credential material out of application logs, crash reports, telemetry, and support bundles. Provide revoke/rotate functionality and audit non-secret events.
- The six-digit PIN has low entropy: short expiry, single use, rate limiting, TLS, and operator verification are mandatory. It is not sufficient by itself as a long-term authentication mechanism.

## Mosquitto lifecycle requirements

1. Detect whether a supported Mosquitto version is installed.
2. If absent, request/perform an explicit install from the official distribution source and verify the executable/version and service status.
3. Render configuration from a safe template; bind only to the selected LAN interface, disable anonymous access, enable TLS and authentication, and enable the selected ACL/dynamic-security mechanism.
4. Validate the configuration and run a local health/connection test before reporting ready.
5. If any security prerequisite fails, do not start an open broker; show a clear diagnostic and keep pairing/remote commands disabled.
6. Do not overwrite an existing user's Mosquitto configuration without backup and explicit consent. Support clean uninstall/rollback.

## Acceptance tests before release

- Correct PIN pairs once; expired, reused, malformed, and incorrect PINs fail.
- Brute-force attempts are rate-limited; PIN and credential values never appear in logs.
- Each device receives a distinct credential and can access only its permitted topics.
- Revoked credentials fail on a new connection; rotation works without leaking the old key.
- Unknown/untrusted TLS certificates are rejected.
- Broker is not reachable from unintended interfaces; anonymous clients are rejected.
- Malformed, stale, duplicate, unauthorized, and retained command messages are rejected.
- Broker/service restart, Wi-Fi loss, phone reconnect, and SD logging during disconnect are tested.
- MQTT loss cannot bypass onboard stabilization or failsafe.
- Record the tested Mosquitto version, configuration hash, test output, and target OS/build in CI or release evidence.

## Implementation boundary

This document is a design specification only. The current Go bridge is a minimal health/placeholder service; this design does not imply that pairing, credential provisioning, TLS, ACLs, or Mosquitto auto-install are already implemented.
