# Contributing to ARES

Thank you for helping improve ARES. Contributions should make the project more testable, maintainable, secure, and safe.

## Before you start

1. Read the [README](README.md), relevant documentation under `docs/`, and the applicable license notices in [LICENSES.md](LICENSES.md).
2. Check open issues and existing code before proposing a new subsystem or dependency.
3. For substantial changes, open an issue or discussion first so the architecture and scope can be agreed upon.

## Engineering expectations

- Treat the repository implementation as the source of truth. Do not document planned features as implemented.
- Keep changes focused and modular; explain interface, data-format, dependency, and migration impacts.
- Add or update tests for changed behavior. Include reproducible commands and observed results in the pull request.
- Never commit API keys, passwords, MQTT credentials, private certificates, personal data, model weights, or generated build artifacts.
- Do not weaken validation, authentication, TLS, access control, or safety checks merely to make CI pass.
- Do not claim that a sensor, model, or subsystem works without test evidence.
- AI/ML outputs are advisory evidence only. Changes must not give an AI model direct authority over flight stabilization, motor PWM, or hardware failsafes.
- Hardware changes must document voltage levels, current/power requirements, interfaces, grounding, timing, and relevant failure modes.

## Pull request checklist

- [ ] The change has a clear purpose and bounded scope.
- [ ] Documentation and comments match the actual implementation.
- [ ] Relevant automated tests, formatters, and static checks have been run.
- [ ] Hardware-dependent checks are explicitly marked as not run when hardware was unavailable.
- [ ] No secrets or unrelated generated files are included.
- [ ] Safety, security, compatibility, and rollback implications are described.

## Commit messages

Use concise, action-oriented messages. A lightweight convention is:

- `feat:` new functionality
- `fix:` bug fix
- `docs:` documentation
- `test:` tests
- `build:` build/dependency changes
- `ci:` continuous integration
- `refactor:` behavior-preserving restructuring

## Review and merge

A pull request may be asked to change scope, add tests, or provide evidence. CI success is necessary where configured, but it does not prove hardware integration, flight readiness, or survivor-detection performance.
