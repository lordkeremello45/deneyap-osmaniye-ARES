# HWcontrol2.0 Reference: ARES Adoption Plan

**Purpose:** Reuse proven engineering practices from [HWcontrol2.0](https://github.com/lordkeremello45/HWcontrol2.0) without importing its desktop-hardware-control architecture into ARES. ARES remains the implementation source of truth.

## Reviewed reference changes

Recent HWcontrol2.0 commits (2026-10-16) add live audio loudness (LUFS) metrics, expose them through probe status, add UI labels and tests, reset metrics between sessions, and suppress stale LUFS values after analysis-queue drops. Other recent commits bound untrusted device metadata before model prompts and test rejection of oversized metadata.

Useful pattern: compute metrics in a worker, publish explicit status, reset session-scoped state, suppress stale results after data loss, bound untrusted inputs, and test both normal and failure paths.

## Adopt for ARES

1. **Telemetry validity and freshness**
   - Every sample/frame carries source ID, sequence number, timestamp, units, validity/quality flags, and sensor status.
   - Distinguish valid, stale, missing, invalid, and sensor_error; never represent missing telemetry as zero or healthy.
   - On queue overflow, timestamp expiry, sequence gaps, or acquisition failure, flag the affected stream and suppress derived outputs that depend on invalid/stale input.
   - Reset rolling metrics and fusion windows at explicit session/mission boundaries.

2. **Bounded, isolated processing**
   - Keep sensor acquisition and raw logging independent from AI/model inference.
   - Use bounded queues and explicit drop counters; avoid unbounded memory growth.
   - Validate payload size, numeric finiteness, timestamps, units, and allowed ranges before parsing deeply or passing data to AI.
   - Treat model output as advisory and retain the underlying measurements and quality flags.

3. **Test-driven failure handling**
   - Add tests for normal frames, missing/stale samples, sequence gaps, queue drops, invalid numeric values, oversized payloads, sensor reconnects, and session resets.
   - Test that invalid inputs cannot produce a valid-looking detection/fusion result.
   - Report actual CI/test evidence; a successful build alone does not establish sensor or flight readiness.

4. **Diagnostics and reproducibility**
   - Log structured events with timestamps, source/component, severity, error code, and counters.
   - Keep raw sensor data distinct from derived metrics and AI interpretations.
   - For a tested build, record commit SHA, toolchain/dependency versions, test commands/results, hardware revision, wiring/interface details, and calibration/configuration version.

5. **Release integrity**
   - For distributable software, generate SHA-256 checksums and consider GitHub artifact attestations/SBOM after the build workflow and permissions are reviewed.
   - Verify attestations rather than merely publishing them. Checksums establish file equality, not publisher identity or software safety.
   - Do not claim hardware support, signing, model inference, or detection capability without direct evidence.

## Do not copy blindly

- HWcontrol2.0's Flutter/Dart dashboard → Go bridge → C++ engine structure is specific to its desktop monitoring product. ARES already targets Deneyap firmware, a C++ AI core, a Go host bridge, and a planned Flutter operator app; preserve those boundaries unless repository evidence justifies a change.
- LUFS is an audio loudness metric, not a survivor-detection feature. For ARES, only add acoustic features after choosing a defined signal-processing method and validating it against recorded/test data.
- Do not add HWcontrol2.0 dependencies or transplant files without checking licensing, target platform, memory/CPU budget, interfaces, and tests.
- AI must not control motor PWM, flight stabilization, or hardware failsafes.

## Implementation order

1. Confirm current telemetry frame contract and logging format in firmware/ and host validation.
2. Add explicit freshness/quality states and queue/drop diagnostics without breaking existing interfaces.
3. Add unit tests for stale, missing, dropped, malformed, and session-reset cases.
4. Add reproducible CI checks and test-result artifacts.
5. Only then consider release checksum/attestation improvements based on the actual release workflow.

## Acceptance criteria

- Invalid, stale, or missing data is visibly distinct from a real zero measurement.
- Queue/data loss invalidates dependent derived metrics until valid input resumes.
- Session reset clears session-scoped metrics and fusion state.
- Malformed/oversized input is rejected before AI inference.
- Tests cover each behavior and are run in CI; hardware-dependent checks are clearly marked as not run.
- Documentation matches implemented behavior and states remaining limitations.

## Sources

- [HWcontrol2.0 repository](https://github.com/lordkeremello45/HWcontrol2.0)
- [Recent LUFS queue-drop fix](https://github.com/lordkeremello45/HWcontrol2.0/commit/09039b2d13888324aa3037bc41d652cfd7daed07)
- [Recent input-size validation test](https://github.com/lordkeremello45/HWcontrol2.0/commit/3b9b39eaa69ae0ab3d78bbb7c6dbadb03f36ba7d)
- [GitHub artifact attestations documentation](https://docs.github.com/en/actions/concepts/security/artifact-attestations)
