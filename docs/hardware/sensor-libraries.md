# Sensor library integration — Deneyap Kart V2

The PlatformIO environment currently uses `board = deneyapkart` with Arduino framework. The official DENEYAP Kart V2 product page identifies an ESP32-S3 module, so the board manifest/IDF target must be checked against the physical board revision before treating the target as confirmed. Driver choices below distinguish selected candidates from integrated/tested code:

| Hardware | Installed driver | Version / source | Integration status |
|---|---|---|---|
| FLIR Lepton 3.5 | Lepton-FLiR-Arduino | 2.1.1, pinned to upstream commit 8577d336ecfc12dc8f3d0612cf8b6dcd681d626f — https://github.com/NachtRaveVL/Lepton-FLiR-Arduino | Dependency installed; verify exact breakout, SPI/VoSPI timing, I2C CCI and radiometry on the actual board |
| Garmin LIDAR-Lite v3 | Garmin LIDAR-Lite | 3.0.6 — https://registry.platformio.org/libraries/garmin/LIDAR-Lite | Official Garmin library; dependency installed; confirm I2C wiring, power and range readings on hardware |
| Qorvo DWM3000 | Selected candidate: `br101/dw3000-decadriver-source` at `67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc` | Qorvo DW3xxx source driver 08.02.02 base; ESP-IDF 5.1.4 port; https://github.com/br101/dw3000-decadriver-source/commit/67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc | Not yet integrated into this Arduino-only environment. Preferred route: evaluate ESP-IDF + Arduino component build, vendor/pin a reviewed upstream commit, preserve Qorvo license notice, then add board-specific SPI/GPIO port. Do not add the repo as a plain `lib_deps` entry and assume it builds. Never substitute a DW1000-only driver. Verify `dwt_readdevid()` (expected DW3110 DEVID `0xDECA0302`) before ranging tests |
| XMOS XVF3800 | No Arduino sensor library | Official firmware/host tools: https://github.com/respeaker/reSpeaker_XVF3800_USB_4MIC_ARRAY | USB version requires a USB host and USB Audio Class capture; Deneyap Kart V2 must not be treated as a USB host. I2S integration requires the matching I2S firmware, verified pinout and clock/master-slave configuration |
| Geospace GS-One LF | No generic library | Analog sensor | Requires a compatible low-noise analog front end, gain/filter design, ADC range and calibration |
| 74LVC2G17 | None | Hardware Schmitt-trigger buffer | Check supply and input/output logic levels; it is not a software driver or bidirectional level shifter |
| 32 GB SD card | ESP32 Arduino SD/SPI APIs | Included in the Arduino-ESP32 framework | No extra library required; wiring/CS pin and filesystem format must be verified |

## XMOS host integration decision

Default architecture: if the physical XVF3800 assembly is the USB/UA variant, connect it to the Windows companion host as a USB Audio Class 2.0 device; do not connect it as a USB peripheral to the DENEYAP board. If the inventory is only the XVF3800 chip plus four microphones, the required PCB, power rails, boot flash/firmware, USB PHY routing and connector must be designed before this path exists. The alternate INT/I2S firmware needs an I2S-capable host and matching clock/master-slave wiring. These are alternative firmware configurations, not simultaneous interfaces.

## Why the old dependencies were removed

The previous Adafruit MLX90640 package targets an MLX90640 thermal array, not the selected FLIR Lepton 3.5. The previous SparkFun VL53L1X package targets a different time-of-flight sensor, not the Garmin LIDAR-Lite v3. Keeping either would create a misleading firmware manifest.

## Reproducibility and verification

- Registry dependencies are pinned to exact stable versions in platformio.ini.
- src/sensor_library_smoke.cpp makes CI compile the Lepton and Garmin public headers so dependency/API breakage is caught by the firmware build.
- A successful compile is not evidence of successful sensor operation. Bench validation still needs a known-good Lepton image/temperature reference and measured LIDAR distances against a reference target.
- Do not hard-code sensor GPIOs until the exact breakout/module revisions and Deneyap Kart V2 pinout are confirmed.
- UWB driver candidate is now identified but not integrated or build-verified. Port it to the actual ESP32-S3/Arduino-or-IDF target, review Qorvo licensing, verify the chip ID and only then run two-node ranging/calibration. Do not claim ranging is operational before those tests.
