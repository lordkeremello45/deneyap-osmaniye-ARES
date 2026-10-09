# Windows Ground Station — preflight and CI validation

## Current status

The Windows Ground Station, MQTT pairing flow, and automatic Mosquitto provisioning are planned integration work; they are not yet an operational end-to-end product. The checks in this directory are diagnostic and build validation, not a security certification.

## Local preflight

Run in PowerShell from the repository root:

```powershell
./scripts/windows/ares-doctor.ps1 -Mode Developer
```

The script is read-only: it does not install dependencies, start services, alter firewall rules, or change broker configuration. It checks Windows/PowerShell, required developer tools, available memory/disk, optional model presence, and Mosquitto availability/state. It writes a sanitized `ares-windows-preflight.json` report in the current directory and does not intentionally record secrets.

To make Mosquitto presence/service a blocking prerequisite for a later runtime test:

```powershell
./scripts/windows/ares-doctor.ps1 -Mode Runtime -RequireMqtt
```

A running Mosquitto service alone does **not** prove secure configuration. TLS certificate validation, anonymous-access rejection, ACL isolation, broker bind scope, PIN rate limiting, and credential revocation require separate integration tests before MQTT is enabled for real commands.

## CI coverage

`.github/workflows/windows-validation.yml` uses a Windows 2022 runner and checks:
- Windows environment preflight and sanitized report shape;
- CMake build + CTest for telemetry;
- CMake build for the C++ AI core;
- Go formatting, vetting, tests, and bridge build.

This deliberately tests only components that exist in the repository. It does not claim that Gemma inference, sensor hardware, Mosquitto provisioning, TLS pairing, or flight-control behavior has been implemented or verified.

## Reference-repository practices adapted

The reference HWcontrol2.0 repository includes dedicated Windows validation, release artifact integrity checks, and diagnostic/test workflows. ARES adopts the relevant principles—platform-specific CI, explicit failure propagation, test reports, and read-only preflight—without copying HWcontrol2.0's implementation or assuming ARES has the same product features.
