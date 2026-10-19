# ARES dual-board firmware build

The project uses two DENEYAP Kart V2 boards with separate firmware images:

- **Card 1 — gateway:** receives bounded newline-delimited JSON from Card 2 and can publish validated sensor frames to MQTT over TLS. MQTT is disabled by default and cannot connect until a local secrets header supplies Wi-Fi, unique broker credentials, host name and a trusted root CA.
- **Card 2 — sensor node:** owns sensor acquisition and emits timestamped JSON once per second. Hardware-specific sensor drivers remain disabled until their pins and electrical interfaces are verified.

## Important language/build boundary

The DENEYAP MCU does not interpret Python, Go, or arbitrary source languages at runtime. PlatformIO compiles the firmware's C/C++ into an ESP32-compatible binary. Python is a host-side build orchestrator and XVF3800 UAC2 agent; Go runs on the companion computer. Shared data contracts (versioned JSON over the local UART/MQTT path) are the inter-language interface—not a runtime source-code translator.

## Build

Install a supported PlatformIO Core and run from the repository root:

    python firmware/build_firmware.py --role both

Other choices are --role card1 and --role card2. The script invokes PlatformIO for the named environment, verifies that each output binary exists, and writes SHA-256 hashes and build metadata into the ignored firmware/build_artifacts/ directory. It does not flash either board.

Equivalent direct commands:

    pio run -d firmware -e ares-card1-gateway
    pio run -d firmware -e ares-card2-sensors

## Local board configuration

1. Copy include/ares_board_config.example.h to include/ares_board_config.h.
2. Set ARES_LINK_RX_PIN and ARES_LINK_TX_PIN to **verified raw GPIO numbers**, not D-labels. Cross-connect TX to RX and share GND. Start with 115200 baud and short wires.
3. Keep sensor-enable macros unset until the exact module/breakout, voltage levels, power and pin assignments are verified.
4. To enable MQTT only, copy include/ares_secrets.example.h to include/ares_secrets.h, fill the configuration, and uncomment ARES_MQTT_ENABLED. Never use setInsecure(); TLS certificate validation is mandatory.
5. Give Card 1 a unique broker identity that can publish only to its assigned telemetry topic. This firmware subscribes to no command topics and is telemetry-only.
6. Build both roles and bench-test the UART protocol before connecting the broker.

The real local headers are git-ignored. .gitignore is not a secret-storage mechanism; protect local files and the physical device. Do not claim flash-secret protection until secure boot/flash encryption have been configured and physically tested.

## Current limits

- This is a first dual-role build scaffold, not flight-ready firmware.
- The exact board revision and inter-board GPIOs are unresolved; the UART path remains disabled until configured.
- MQTT TLS logic is implemented behind an explicit local configuration gate, but broker provisioning, per-device ACL setup and end-to-end TLS tests remain outstanding.
- FLIR output is a raw center-pixel TLinear value, not calibrated Celsius. LiDAR and geophone paths need physical validation. DWM3000 is not integrated; XVF3800 UAC2 is currently host-side.
- No motor control, flight stabilization, or replacement for an independent flight controller/failsafe is provided.
