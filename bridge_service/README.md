# ARES Go bridge service

The bridge is a local companion-host service. It must not be exposed directly to a LAN or the public Internet.

## Run locally

From the repository root, use PowerShell:

    $env:ARES_BRIDGE_TOKEN = python -c "import secrets; print(secrets.token_urlsafe(32))"
    cd bridge_service
    go test ./...
    go run .

Keep the same ARES_BRIDGE_TOKEN in the environment of the host process running the XVF3800 UAC2 agent. The token is not a firmware credential and must never be committed or printed to logs. If separate shells are used, transfer it through a protected local environment/configuration mechanism.

The service refuses to start if the token is absent or shorter than 32 characters. Data endpoints require an Authorization header with a Bearer token. The health endpoint is intentionally unauthenticated and returns no telemetry. The listener accepts only a numeric loopback IP; the default is 127.0.0.1:8080.

## Current API state

- GET /health — process health only.
- GET /api/v1/telemetry — authenticated, currently returns 503 waiting_for_device until a serial telemetry adapter is implemented.
- POST /api/v1/audio/rms — authenticated, accepts bounded, validated JSON summaries only.
- GET /api/v1/audio — authenticated, returns the most recent fresh RMS/peak summary.

This bridge is not the MQTT broker and does not implement pairing or remote TLS. Do not bind it to a LAN interface. Remote access requires a separately designed authenticated TLS service and explicit threat review.
