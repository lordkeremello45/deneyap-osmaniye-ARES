# ARES firmware

Target: Deneyap Kart V2 / classic ESP32 / Arduino framework, built with PlatformIO board ID `deneyapkart`.

## Installed sensor libraries

- **FLIR Lepton 3.5:** `NachtRaveVL/Lepton-FLiR-Arduino` v2.1.1, pinned to commit `8577d336ecfc12dc8f3d0612cf8b6dcd681d626f`. Provides Lepton CCI/VoSPI access. Actual breakout wiring, SPI timing, reset/VSYNC pins, radiometry mode and thermal reference validation remain hardware tasks.
- **Garmin LIDAR-Lite v3:** `garmin/LIDAR-Lite@3.0.6`, the manufacturer's Arduino library. The VL53L1X driver is not appropriate for this sensor.
- **SD card:** ESP32 Arduino SD/SPI APIs are part of the framework; no extra sensor library is required.

A compile-only smoke translation unit includes the Lepton and Garmin public headers so CI catches dependency-resolution and API compile failures.

## Components without a generic Arduino library

- **Qorvo DWM3000:** no driver is selected until a licensed DW3000-specific port is verified for the exact module and Deneyap Kart V2. Do not substitute DW1000-only libraries. SPI/IRQ/reset pins, antenna/module revision and two-node ranging must be validated before use.
- **XMOS XVF3800:** USB firmware requires a USB host and USB Audio Class capture; Deneyap Kart V2 is not assumed to be that host. I2S use requires the matching XVF3800 I2S firmware plus verified I2S clock, channel and pin configuration.
- **Geospace GS-One LF:** analog signal source; requires a low-noise analog front end, filtering, suitable ADC input range and calibration.
- **74LVC2G17:** hardware Schmitt-trigger buffer; no software driver. It is not a bidirectional level shifter.

## Integration safety

- Do not assign GPIOs until the exact module/breakout revisions and Deneyap Kart V2 pinout are verified.
- Do not assume sensor output is calibrated or represents a survivor. Record timestamps, validity/status and calibration metadata.
- A successful CI build confirms compilation only; it does not confirm electrical compatibility, sensor communication, ranging accuracy or survivor-detection performance.
- See [sensor library integration notes](../docs/hardware/sensor-libraries.md) for pinned versions, sources and validation steps.
