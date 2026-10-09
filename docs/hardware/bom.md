# Hardware inventory — candidates, not final selections

| Subsystem | Candidate / requirement | Verification |
|---|---|---|
| Compute | 2x Deneyap Kart V2 | board revision, pinout, logic levels |
| Local storage | 8 MB Octal SPI Flash per board (assumption) | exact IC, access path, usable capacity |
| Interconnect | full-duplex UART | cable length, baud, EMI; consider differential transceiver if needed |
| UWB | Qorvo DWM3000 / DW3110; driver candidate `br101/dw3000-decadriver-source` | driver is ESP-IDF-oriented and not integrated yet; verify board revision, SPI/IRQ/reset, Qorvo license and two-node ranging |
| Thermal | Lepton-class candidate | breakout/interface, calibration, FOV, processing |
| Acoustic | XMOS XVF3800 + 4 PDM MEMS microphones; default host path is USB Audio Class 2.0 to Windows if a UA/USB assembly is present | physical board/firmware variant and power must be confirmed; raw chip + microphones needs a custom PCB; I2S is a separate INT firmware path |
| Seismic | geophone + low-noise analog front end | ADC, gain/filter, calibration and mounting |
| LiDAR | ranging sensor candidate | range, surfaces/light, UART/I2C logic |
| Power | battery + regulated DC/DC rails | peak current, thermal, EMI; isolate motor and logic power |
| Propulsion | T-Motor U8 Lite KV150 remains a candidate; no flight configuration approved | T-Motor product page has separate KV85/KV150 variants and differing 6S/12S vs V-Link 13S/MF24 matching data; see `propulsion-selection.md`; obtain manufacturer confirmation before final ESC/prop/battery selection |
| Frame/guard | fiberglass/composite | mass, vibration, fastener and clearance tests |

A part becomes selected only after recording manufacturer datasheet, exact part number, voltage, interface, current, mass, dimensions, driver/license and test evidence.


## Blocked decisions

- Do not use `Pololu D24V50F5` to power the propulsion system; it is a 5 V logic/sensor buck regulator.
- Do not finalize battery, ESC, propeller, flight controller or PDB until rotor count/layout, measured all-up mass, payload, target hover time and exact motor variant are defined.
- See [propulsion selection and validation plan](propulsion-selection.md) for the temporary bench candidate and flight-release gates.
