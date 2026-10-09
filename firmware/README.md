# ARES firmware

Target: DENEYAP Kart V2; the repository currently declares PlatformIO board ID deneyapkart and Arduino framework. **The declared board target still requires comparison with the exact physical board revision and MCU before GPIO or peripheral mapping is approved.**

## Installed sensor libraries

- **FLIR Lepton 3.5:** NachtRaveVL/Lepton-FLiR-Arduino v2.1.1, pinned to commit 8577d336ecfc12dc8f3d0612cf8b6dcd681d626f. Provides Lepton CCI/VoSPI access. Breakout wiring, SPI timing, reset/VSYNC pins, radiometry mode and thermal reference validation remain hardware tasks.
- **Garmin LIDAR-Lite v3:** garmin/LIDAR-Lite@3.0.6, the manufacturer's Arduino library. The VL53L1X driver is not appropriate for this sensor.
- **SD card:** ESP32 Arduino SD/SPI APIs are part of the framework; no extra sensor library is required.

A compile-only smoke translation unit includes the Lepton and Garmin public headers so CI catches dependency-resolution and API compile failures.

## Components without a verified integrated driver

- **Qorvo DWM3000:** selected driver candidate is [br101/dw3000-decadriver-source at commit 67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc](https://github.com/br101/dw3000-decadriver-source/commit/67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc). This is an ESP-IDF-oriented DW3xxx port, not a PlatformIO Arduino library and is not integrated into this firmware yet. Review its license notices, pin the source, reconcile the exact ESP32-S3 board target, compile the IDF/Arduino combination, check device ID, then perform two-node ranging. Do not substitute DW1000-only libraries.
- **XMOS XVF3800:** default architecture is USB Audio Class 2.0 to the Windows companion host only if the physical device is a matching UA/USB assembly. A bare XVF3800 chip plus microphones requires a designed PCB and firmware. I2S requires the matching firmware variant and verified host clock/channel/pin configuration.
- **Geospace GS-One LF:** analog signal source; requires a low-noise analog front end, filtering, suitable ADC input range and calibration.
- **74LVC2G17:** hardware Schmitt-trigger buffer; no software driver. It is not a bidirectional level shifter.

## Integration safety

- Do not assign GPIOs until exact module/breakout revisions and DENEYAP Kart V2 pinout are verified.
- Do not assume sensor output is calibrated or represents a survivor. Record timestamps, validity/status and calibration metadata.
- CI build success confirms compilation only; it does not confirm electrical compatibility, sensor communication, ranging accuracy or survivor-detection performance.
- See [sensor library integration notes](../docs/hardware/sensor-libraries.md), [hardware review report](../docs/hardware/review-report-for-teacher.md), and [software status](../docs/software/status.md).
