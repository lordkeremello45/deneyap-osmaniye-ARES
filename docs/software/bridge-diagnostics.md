# Bridge Diagnostics and Telemetry Freshness Contract

**Status:** Implemented in the Go bridge; requires CI execution and integration testing before operational use.

## Why this exists

A live HTTP process is not proof that a sensor is connected. ARES therefore separates service liveness from telemetry availability and freshness. This follows established API input-validation and observability practices while preserving the current loopback-only prototype boundary.

## Endpoints

### `GET /health`

Reports whether the HTTP service responds. `status: ok` does not imply sensor connectivity or flight readiness.

### `GET /api/v1/status`

Returns a compact diagnostic snapshot:

- `service`: bridge process response state.
- `telemetry.status`: currently `waiting_for_device`; `available: false` until the full telemetry adapter exists.
- `audio.status`: `waiting_for_xvf3800`, `ok`, or `stale` based on whether an audio summary has arrived and whether its receive age is at most three seconds.
- `audio.available`: true only when a sample is currently fresh.
- `audio.age_ms`: age of the last received audio sample when one exists; null before the first sample.

The endpoint uses HTTP 200 for a valid diagnostic response even if sensors are unavailable. Consumers must inspect the component statuses, not just the HTTP code.

### `GET /api/v1/audio`

- HTTP 200 with `status: ok` and data when the latest audio summary is no more than three seconds old.
- HTTP 503 with `status: waiting_for_xvf3800` before the first sample.
- HTTP 503 with `status: stale`, `data: null`, and `age_ms` after a sample ages out. Stale data is not returned as current telemetry.

### `POST /api/v1/audio/rms`

Accepts only the documented XVF3800 UAC2 RMS/peak summary. The handler caps the body at 4096 bytes, rejects unknown JSON fields and extra trailing JSON values, validates the source, sample rate, channel count, finite/range-consistent RMS and peak values, and captured timestamp freshness. It does not accept raw audio and does not establish that a sound indicates a survivor.

## Consumer rules

1. Treat `waiting_for_device`, `waiting_for_xvf3800`, and `stale` as unavailable evidence, not as zero-valued sensor readings.
2. Do not fuse stale audio with current thermal, ranging, or vibration observations.
3. `GET /health` and `service: ok` indicate service liveness only. They are not readiness, sensor-health, flight-safety, or survivor-detection claims.
4. The bridge must remain bound to loopback until authenticated remote access and transport security are implemented and verified.

## Verification

Run from `bridge_service/`:

```sh
gofmt -w main.go main_test.go
go test ./...
go test -race ./...
```

The tests cover fresh/unavailable audio, stale-data suppression, invalid sample rejection, unknown JSON fields, status semantics, HTTP method restrictions, and loopback-only binding. CI results must be checked before calling this change verified. Bench tests must still validate the physical XVF3800 host path and end-to-end telemetry.

## Security and engineering references

- [OWASP REST Security Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/REST_Security_Cheat_Sheet.html) — validate inputs, enforce request limits, and apply transport/authentication controls.
- [OpenTelemetry concepts](https://opentelemetry.io/docs/concepts/signals/) — separate observable signals and instrument system behavior; ARES currently implements only a minimal status contract, not full OpenTelemetry instrumentation.
- [ARES software status](status.md) — source-of-truth implementation limits and integration gates.
