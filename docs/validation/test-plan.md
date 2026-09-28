# Validation and safety test plan

Bench: verify pinout, logic level, polarity; UART loopback; framing/CRC/sequence/ACK/retry; measure idle and peak current and temperature.

Hardware-in-loop: simultaneous bidirectional traffic; sensor simulation; latency/loss; Flash read/write and power-loss behavior; watchdog/reboot.

Mechanical/EMI: motors and ESCs tested with propellers removed; measure packet errors and sensor drift; inspect frame, fasteners, vibration and cable restraint.

Field: verify independent flight-controller failsafes; use permitted controlled test area; preflight checklist; low-risk profiles first. ARES detections are operator-support indications, never sole rescue/flight decisions.

Define numeric acceptance thresholds before testing and attach reports/evidence.
