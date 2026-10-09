# Security Policy

ARES is a research and prototyping project. It must not be assumed safe or ready for operational search-and-rescue use.

## Reporting a vulnerability

Please do not publish exploitable details, credentials, private keys, or sensitive operational data in a public issue.

Use GitHub's **Report a vulnerability** feature on this repository when it is available. If private vulnerability reporting is unavailable, contact the repository maintainer through the contact method listed on the maintainer's public GitHub profile and provide only the minimum details needed to establish the issue. Avoid sending secrets or personal data.

Include, where possible:

- affected component and revision;
- impact and prerequisites;
- concise reproduction steps or a proof of concept;
- logs with credentials, tokens, and personal data removed;
- a suggested mitigation, if known.

## Security-critical areas

Treat the following as security-sensitive:

- Bluetooth onboarding and device identity verification;
- MQTT authentication, TLS, topic ACLs, credential rotation and revocation;
- secret storage and provisioning on Windows and embedded devices;
- telemetry validation and malformed-message handling;
- dependency and build-pipeline integrity.

Never use default or shared production credentials, anonymous MQTT access, or an unauthenticated fallback. Do not put secrets in source control, logs, screenshots, or issue reports.

## Response expectations

The maintainers will assess reports as time and resources permit. No fixed response or patch deadline is guaranteed. Please allow time for investigation and coordinated disclosure before publishing details.
