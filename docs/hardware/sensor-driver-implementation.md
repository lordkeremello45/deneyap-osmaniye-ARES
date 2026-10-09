# ARES firmware sensor-driver status

A shared sensor-driver API now reports timestamped JSON status at 1 Hz. Drivers are disabled by default; no unverified GPIO mapping is assumed.

- FLIR Lepton 3.5: uses the pinned Lepton-FLiR-Arduino frame API. Reports the center pixel's raw TLinear value only when a frame is available and TLinear mode is enabled. This raw value is not Celsius until radiometry resolution and module configuration are verified.
- Garmin LIDAR-Lite v3: uses Garmin's driver; positive centimetre readings are converted to millimetres; non-positive values are invalid.
- Geospace GS-One LF: optional raw ADC sampling only. This is not calibrated vibration data and requires an approved analog front end and ADC range.
- Qorvo DWM3000: reports not_integrated until the DW3xxx ESP-IDF driver is ported, device ID verified, and two-node ranging tested.
- XMOS XVF3800: reports not_integrated until the physical USB/UA versus bare-chip variant and USB Audio/I2S interface are confirmed.
- SD card: framework API exists; logging is not enabled until a verified chip-select pin is assigned.

## Enabling onboard drivers
After verifying the exact board revision, electrical levels and module wiring, configure build flags in firmware/platformio.ini:
- ARES_I2C_SDA_PIN and ARES_I2C_SCL_PIN: verified GPIO numbers.
- ARES_ENABLE_LIDAR to enable the Garmin driver.
- ARES_ENABLE_LEPTON and ARES_LEPTON_CS_PIN to enable the Lepton driver.
- ARES_ENABLE_GEOSPACE_ADC and ARES_GEOSPACE_ADC_PIN only after analog front-end review.

Do not use placeholders as literal compiler flags. Test each driver independently. A successful compile is not proof of physical operation or calibrated survivor detection.
