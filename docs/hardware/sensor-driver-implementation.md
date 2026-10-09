# ARES firmware sensor-driver status

The shared acquisition layer reports timestamped JSON status at 1 Hz. GPIO-dependent firmware drivers remain disabled until the exact DENEYAP board revision and module wiring are verified.

## Driver status

- **FLIR Lepton 3.5:** uses the pinned Lepton-FLiR-Arduino frame API. Reports the center pixel's raw TLinear value only when a frame is available and TLinear mode is enabled. This raw value is not Celsius until radiometry resolution and module configuration are verified.
- **Garmin LIDAR-Lite v3:** uses Garmin's driver; positive centimetre readings are converted to millimetres; non-positive values are invalid.
- **Geospace GS-One LF:** optional raw ADC sampling only. This is not calibrated vibration data and requires an approved analog front end and ADC range.
- **Qorvo DWM3000:** do not use a DW1000 library. The preferred reference implementation is [br101/dw3000-decadriver-source at commit 67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc](https://github.com/br101/dw3000-decadriver-source/commit/67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc), which contains Qorvo DW3xxx driver source and an ESP-IDF 5.1.4 platform layer. The current firmware target is PlatformIO/Arduino, so this driver is **not yet linked into the active firmware build**. Integrate it as an ESP-IDF component (with the Arduino component retained only if the other sensor libraries need it) or complete and validate a deliberate Arduino port. Do not report a distance until the driver is initialized, the device ID matches DW3110 (expected DWM3000 DEVID 0xDECA0302), and a two-node DS-TWR exchange passes tests. The inventory currently lists only one DWM3000, so physical ranging validation also requires a second compatible UWB node.
- **XMOS XVF3800:** the chosen interface is the official USB Audio Class 2.0 (UAC2) configuration when the physical unit has matching UA/USB firmware. The host-side agent is in `host_tools/xvf3800_uac2/`; it selects an explicitly identified XMOS/VocalFusion input device, computes normalized RMS/peak summaries in memory, and posts only those summaries to the local Go bridge. It does not store or upload raw audio. The Go bridge exposes `POST /api/v1/audio/rms` and `GET /api/v1/audio`. A bare XVF3800 chip is not USB-ready without the required PCB, firmware, clocking, flash and USB circuitry. USB and I²S are mutually exclusive build-time firmware configurations.
- **SD card:** framework API exists; logging is not enabled until a verified chip-select pin is assigned.

## XMOS host-agent setup

See [XVF3800 UAC2 host agent](../../host_tools/xvf3800_uac2/README.md). Verify the actual device name, firmware sample rate and channel layout before starting capture. The agent deliberately refuses to capture an arbitrary microphone.

## Enabling onboard drivers

After verifying the exact board revision, electrical levels and module wiring, configure build flags in `firmware/platformio.ini`:
- `ARES_I2C_SDA_PIN` and `ARES_I2C_SCL_PIN`: verified GPIO numbers.
- `ARES_ENABLE_LIDAR` to enable the Garmin driver.
- `ARES_ENABLE_LEPTON` and `ARES_LEPTON_CS_PIN` to enable the Lepton driver.
- `ARES_ENABLE_GEOSPACE_ADC` and `ARES_GEOSPACE_ADC_PIN` only after analog front-end review.

Do not use placeholders as literal compiler flags. Test each driver independently. A successful compile is not proof of physical operation, ranging accuracy, calibrated acoustic features or survivor-detection performance.
