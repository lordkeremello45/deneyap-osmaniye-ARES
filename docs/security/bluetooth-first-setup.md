# ARES Windows Bluetooth-first provisioning

**Status: design only; not implemented or hardware-validated.**

## Goal

During Windows Ground Station setup, guide the operator through Bluetooth discovery and first-time device enrollment. After the operator selects and confirms the intended drone, Windows provisions that node with its own MQTT identity automatically. The user must not need to view, copy, or type the MQTT secret.

## Mandatory hardware gate

Before implementing discovery, verify the exact MCU and radio on each DENEYAP Kart V2 revision and the actual firmware support. Do not assume that a board marketed as ESP32 has Bluetooth: ESP32-S2 does not provide Bluetooth. If the installed board revision lacks Bluetooth, select and validate a compatible external BLE module or use a different authenticated provisioning transport. Do not silently fall back to insecure Wi-Fi or plaintext UART.

## Target wizard

1. **Prepare:** verify Ground Station prerequisites, broker configuration, TLS certificate, and pairing service health. Do not advertise pairing-ready until security checks pass.
2. **Discover:** scan for ARES provisioning advertisements. Display a stable device identity and, when available, a user-verifiable identifier. Do not trust a Bluetooth name alone.
3. **Confirm physical device:** require an explicit operator selection plus proof of physical presence, such as a short-lived pairing window activated by a hardware button. If a button is not available, design and validate an alternative such as a per-device QR bootstrap secret. A six-digit PIN or device name alone is not strong device authentication.
4. **Authenticate channel:** use authenticated BLE Secure Connections where the selected hardware and stack support it, with a validated association method. Bluetooth pairing by itself is not automatically proof that the intended device is genuine. Use a reviewed challenge-response/enrollment protocol and reject downgrade or unauthenticated modes.
5. **Issue device identity:** Windows generates a unique, high-entropy MQTT credential for this node and registers least-privilege ACLs. Never reuse the phone's identity or a fleet-wide key.
6. **Transfer securely:** send the credential only inside the authenticated, encrypted provisioning session. Bind the exchange to the device identity and a fresh nonce/session to prevent replay. Do not include secrets in BLE advertisements, debug output, logs, telemetry, or crash reports.
7. **Persist:** store the credential using the strongest storage protections actually supported by the validated board/MCU. Enable and validate secure boot and flash encryption if supported by that exact target. Until physical extraction resistance is tested, do not claim the secret is protected against physical access.
8. **Verify:** the node connects to the configured broker using TLS with certificate validation. Test that its credential can access only its own permitted topics. The wizard reports success only after broker-side authorization and device identity checks pass.
9. **Recover:** on interruption, make enrollment idempotent and time-limited. Revoke abandoned credentials, avoid creating unlimited orphan identities, and allow operator-initiated revoke/re-pair. A new pairing creates a new credential; it must not reveal the old one.

## Failure behavior

- Missing/incompatible Bluetooth hardware, unsupported secure pairing, invalid broker TLS, failed ACL creation, or unverified device identity must stop provisioning with an actionable error.
- Never disable TLS validation, enable anonymous MQTT, use a shared fleet credential, or send a permanent key over plaintext transport as a workaround.
- Pairing and MQTT provisioning must not control flight stabilization, motor PWM, or onboard failsafe. Loss of Windows/Bluetooth/Wi-Fi/MQTT must not impair those independent safety functions.

## Acceptance tests before implementation is considered complete

- Unsupported board revision is detected and reported without entering insecure fallback.
- Only the physically authorized, authenticated node can enroll.
- MITM, replay, downgrade, malformed messages, expired enrollment windows, and repeated failed attempts are rejected.
- Each node gets a distinct credential and is restricted by broker ACLs.
- Credential transfer and logs contain no plaintext secret.
- Reboot/power-loss during provisioning recovers safely without duplicate or orphan credentials.
- Revocation blocks the old credential; re-pairing creates a new one.
- TLS certificate failures and broker authorization failures make the wizard report failure.
- Firmware, BLE stack, storage protections, and provisioning protocol are tested on the actual board revision.

This specification describes intended behavior only. It does not claim Bluetooth hardware support, secure provisioning, MQTT integration, or secret-at-rest protection is currently implemented.
