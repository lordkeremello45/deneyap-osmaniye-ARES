# Ada/SPARK safety-logic workstream

Ada/SPARK is introduced as a **separate formal-verification workstream**, not as a replacement for the existing C++ AI core, Go bridge, Flutter application, or ESP32 firmware.

## Initial scope

- Express bounded sensor-reading validation and conservative mission-state selection in a small SPARK package.
- Keep sensor values untrusted until transport validation, freshness, range checks, and quality flags pass.
- Keep flight stabilization, motor PWM, and hardware failsafe outside the AI model and outside this initial prototype.
- Prove package contracts with GNATprove when an Ada/SPARK toolchain is available; do not claim proof until command output is archived.

## Run

Install an Ada compiler and SPARK/GNATprove toolchain appropriate to your platform, then run:

```sh
gprbuild -P ares_safety.gpr
gnatprove -P ares_safety.gpr --mode=prove
```

Toolchain availability and successful proof have not yet been verified in CI. The example is deliberately conservative and does not claim validated survivor detection.
