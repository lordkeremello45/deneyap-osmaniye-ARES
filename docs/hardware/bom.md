# Hardware inventory — candidates, not final selections

| Subsystem | Candidate / requirement | Verification |
|---|---|---|
| Compute | 2x Deneyap Kart V2 | board revision, pinout, logic levels |
| Local storage | 8 MB Octal SPI Flash per board (assumption) | exact IC, access path, usable capacity |
| Interconnect | full-duplex UART | cable length, baud, EMI; consider differential transceiver if needed |
| UWB | module candidate | legal band, antenna, SPI, certification |
| Thermal | Lepton-class candidate | breakout/interface, calibration, FOV, processing |
| Acoustic | microphone array + codec/processor | sample rate, direction finding, vibration/noise |
| Seismic | geophone + low-noise analog front end | ADC, gain/filter, calibration and mounting |
| LiDAR | ranging sensor candidate | range, surfaces/light, UART/I2C logic |
| Power | battery + regulated DC/DC rails | peak current, thermal, EMI; isolate motor and logic power |
| Propulsion | U8 Lite KV150 discussed as candidate | motor/ESC/propeller thrust table, MTOW and guarded-prop test |
| Frame/guard | fiberglass/composite | mass, vibration, fastener and clearance tests |

A part becomes selected only after recording manufacturer datasheet, exact part number, voltage, interface, current, mass, dimensions, driver/license and test evidence.
