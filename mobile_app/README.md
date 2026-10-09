# ARES Mobile — planned phase

Flutter application for a future Android/iOS operator interface. **Mobile development is not the current implementation priority.**

Current priority:
1. Deneyap Kart V2 firmware and sensor acquisition.
2. Timestamped, validated telemetry and raw-data logging.
3. Companion-host AI pipeline and Gemma 4 E2B Q5_K_M inference validation.
4. Only then choose and implement an operator link/UI.

Possible future transports are Deneyap BT/BLE for local low-volume telemetry or MQTT over Wi-Fi/IP when a brokered network is justified. Neither has been selected or implemented as the final transport. Do not treat the mobile app or its link as operational until the protocol, authentication, reconnect behavior, latency, and packet-loss behavior are tested.

Planned architecture: Flutter/Dart UI ↔ validated Go Bridge/API ↔ C++ AI Core on the companion host. This is a target design, not a claim that an end-to-end mobile integration currently exists.
