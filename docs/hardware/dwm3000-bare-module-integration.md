# ARES — Bare Qorvo DWM3000 module integration plan

## Selected software baseline

- **Radio/driver:** Qorvo DW3xxx source driver 08.02.02 from `br101/dw3000-decadriver-source`, pinned to commit `67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc`.
- **Initial platform target:** ESP-IDF 5.1.4, matching the selected driver's provided ESP-IDF platform layer.
- **Why not add it to the current PlatformIO Arduino build immediately:** the selected driver is an ESP-IDF component, not a drop-in Arduino library. The current firmware also uses Arduino-oriented Lepton and Garmin libraries. Combining these without an explicit, compiled integration path risks breaking the firmware build or misconfiguring the SPI/GPIO layer.
- **No fabricated ranging:** the existing firmware telemetry remains `not_integrated` until device-ID validation and a real ranging test succeed.

## Physical integration — bare module

The DWM3000 is a 24-pin castellated RF module, not a breadboard-ready breakout. Design and verify a carrier PCB before connecting it to DENEYAP Kart V2.

1. Follow the [Qorvo DWM3000 datasheet](https://www.decawave.com/wp-content/uploads/2021/01/DWM3000-Datasheet-1.pdf) for the exact pad numbering, supply rails, bypass capacitors, ground, antenna keep-out, and layout. Do not infer the footprint or pad numbering from a product photograph.
2. Provide the required `VDD3V3` and `VDD1` rails within the datasheet limits. Design the rails and decoupling from the datasheet's operating/transmit-current requirements; do not power the module from the Pololu D24V50F5's 5 V output directly.
3. Route SPI `SCK`, `MOSI`, `MISO`, and active-low `CS`, plus `IRQ`, `RESETn`, and `WAKEUP` to verified host GPIOs. The module is an SPI slave. Ensure MISO is high-impedance when CS is inactive.
4. Confirm the DENEYAP board's exact MCU and GPIO electrical limits, then confirm the selected pins do not conflict with flash, USB, boot strapping, SD, Lepton or other sensors. Do not copy example pin numbers from an ESP32-S3 forum post.
5. Check the carrier PCB's antenna keep-out and ground layout against Qorvo guidance. Do not place carbon fibre, batteries, wiring bundles or other conductive structures in the antenna's keep-out region.

## Firmware implementation gates

1. Create a separate ESP-IDF target/component integration and preserve Qorvo's license notices. Do not vendor a partial driver source set or substitute a DW1000-only library.
2. Configure the driver using actual carrier schematic pin assignments; keep unverified GPIOs out of the default build.
3. Initialize SPI and GPIO, reset the module, and read the DW3110 device ID. Expected DEVID for DWM3000 is `0xDECA0302`. A wrong ID or failed read is a hard initialization failure, not a distance of zero.
4. Only after reliable ID reads, validate interrupt handling and packet RX/TX with a second compatible UWB node.
5. Implement a timestamped DS-TWR exchange with timeout, frame validation, sequence checks, range bounds, and explicit invalid/error states. Convert to millimetres only after the range computation is validated.
6. Test repeated cold boots, supply droop during TX, SPI error injection where practical, antenna orientation, and range against a measured reference. Record error distribution and outliers.

## Current blockers

- The inventory contains only one DWM3000; a second compatible UWB node is required to validate actual two-way ranging.
- Carrier PCB schematic, exact DENEYAP Kart V2 MCU/revision, and approved GPIO mapping have not been provided.
- The current Arduino firmware target does not yet compile the selected ESP-IDF driver.
- No electrical, RF, or ranging validation has been performed. Until these gates pass, ARES must report DWM3000 as not integrated and must not claim a valid range.

## References

- [Qorvo DWM3000 datasheet](https://www.decawave.com/wp-content/uploads/2021/01/DWM3000-Datasheet-1.pdf)
- [Pinned DW3xxx driver reference](https://github.com/br101/dw3000-decadriver-source/commit/67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc)
- [Qorvo DWM3000 product information](https://www.qorvo.com/products/p/DWM3000)
