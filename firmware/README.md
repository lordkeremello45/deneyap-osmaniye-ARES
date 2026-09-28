# ARES firmware

Target: Deneyap Kart V2 / ESP32 Arduino framework.

## Sensor stack
- Thermal: MLX90640 class; Adafruit MLX90640 library is the initial dependency.
- UWB: DW3000-class module; exact driver is selected after the physical module is finalized.
- Acoustic: INMP441-class I2S microphone; ESP32 I2S peripheral, no extra sensor library required.
- Seismic: geophone + low-noise analog front end; ADC input and calibration.
- LiDAR: VL53L1X-class ToF; SparkFun VL53L1X library.

The BOM still treats exact part numbers as candidates. Pin maps and initialization are locked only after physical verification.
