# Sensor library integration — Deneyap Kart V2

The firmware target is the classic ESP32-based Deneyap Kart (board = deneyapkart). The PlatformIO build pins the latest stable registry releases selected for the physical BOM:

| Hardware | Installed driver | Version / source | Integration status |
|---|---|---|---|
| FLIR Lepton 3.5 | Lepton-FLiR-Arduino | 2.1.1, pinned to upstream commit 8577d336ecfc12dc8f3d0612cf8b6dcd681d626f — https://github.com/NachtRaveVL/Lepton-FLiR-Arduino | Dependency installed; verify exact breakout, SPI/VoSPI timing, I2C CCI and radiometry on the actual board |
| Garmin LIDAR-Lite v3 | Garmin LIDAR-Lite | 3.0.6 — https://registry.platformio.org/libraries/garmin/LIDAR-Lite | Official Garmin library; dependency installed; confirm I2C wiring, power and range readings on hardware |
| Qorvo DWM3000 | No dependency selected yet | No stable, verified PlatformIO package found that can safely be declared compatible with this exact module/board | Do not substitute DW1000-only libraries. Select a licensed DW3000 driver and validate module revision, SPI/IRQ/reset pins and ranging examples before integration |
| XMOS XVF3800 | No Arduino sensor library | Official firmware/host tools: https://github.com/respeaker/reSpeaker_XVF3800_USB_4MIC_ARRAY | USB version requires a USB host and USB Audio Class capture; Deneyap Kart V2 must not be treated as a USB host. I2S integration requires the matching I2S firmware, verified pinout and clock/master-slave configuration |
| Geospace GS-One LF | No generic library | Analog sensor | Requires a compatible low-noise analog front end, gain/filter design, ADC range and calibration |
| 74LVC2G17 | None | Hardware Schmitt-trigger buffer | Check supply and input/output logic levels; it is not a software driver or bidirectional level shifter |
| 32 GB SD card | ESP32 Arduino SD/SPI APIs | Included in the Arduino-ESP32 framework | No extra library required; wiring/CS pin and filesystem format must be verified |

## Why the old dependencies were removed

The previous Adafruit MLX90640 package targets an MLX90640 thermal array, not the selected FLIR Lepton 3.5. The previous SparkFun VL53L1X package targets a different time-of-flight sensor, not the Garmin LIDAR-Lite v3. Keeping either would create a misleading firmware manifest.

## Reproducibility and verification

- Registry dependencies are pinned to exact stable versions in platformio.ini.
- src/sensor_library_smoke.cpp makes CI compile the Lepton and Garmin public headers so dependency/API breakage is caught by the firmware build.
- A successful compile is not evidence of successful sensor operation. Bench validation still needs a known-good Lepton image/temperature reference and measured LIDAR distances against a reference target.
- Do not hard-code sensor GPIOs until the exact breakout/module revisions and Deneyap Kart V2 pinout are confirmed.
- UWB remains intentionally unselected until a compatible, appropriately licensed DW3000 port is verified. Do not claim ranging is operational before two-node tests and calibration.
