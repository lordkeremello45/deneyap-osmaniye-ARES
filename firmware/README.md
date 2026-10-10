# ARES firmware

Target: two DENEYAP Kart V2 boards, with separate PlatformIO environments: ares-card1-gateway and ares-card2-sensors. The board ID remains deneyapkart; compare it with the exact physical board revision and MCU before GPIO or peripheral mapping is approved. See [dual-board build instructions](BUILDING.md) and [architecture](../docs/software/two-board-firmware-architecture.md).

## Installed sensor libraries

- **FLIR Lepton 3.5:** NachtRaveVL/Lepton-FLiR-Arduino v2.1.1, pinned to commit 8577d336ecfc12dc8f3d0612cf8b6dcd681d626f. Provides Lepton CCI/VoSPI access. Breakout wiring, SPI timing, reset/VSYNC pins, radiometry mode and thermal reference validation remain hardware tasks.
- **Garmin LIDAR-Lite v3:** garmin/LIDAR-Lite@3.0.6, the manufacturer's Arduino library. The VL53L1X driver is not appropriate for this sensor.
- **SD card:** ESP32 Arduino SD/SPI APIs are part of the framework; no extra sensor library is required.

A compile-only smoke translation unit includes the Lepton and Garmin public headers so CI catches dependency-resolution and API compile failures.

## Components without a verified integrated driver

- **Qorvo DWM3000:** selected driver candidate is [br101/dw3000-decadriver-source at commit 67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc](https://github.com/br101/dw3000-decadriver-source/commit/67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc). This is an ESP-IDF-oriented DW3xxx port, not a PlatformIO Arduino library and is not integrated into this firmware yet. Review its license notices, pin the source, reconcile the exact ESP32-S3 board target, compile the IDF/Arduino combination, check device ID, then perform two-node ranging. Do not substitute DW1000-only libraries.
- **XMOS XVF3800:** selected interface is USB Audio Class 2.0 (UAC2) to a host computer, provided the physical unit is a matching UA/USB assembly with UAC2 firmware. The host agent in `../host_tools/xvf3800_uac2/` captures audio and sends only RMS/peak summaries to the Go bridge. A bare chip plus microphones requires a designed PCB and firmware. I2S requires a matching firmware variant and verified host clock/channel/pin configuration.
- **Geospace GS-One LF:** analog signal source; requires a low-noise analog front end, filtering, suitable ADC input range and calibration.
- **74LVC2G17:** hardware Schmitt-trigger buffer; no software driver. It is not a bidirectional level shifter.

## Integration safety

- Do not assign GPIOs until exact module/breakout revisions and DENEYAP Kart V2 pinout are verified.
- Do not assume sensor output is calibrated or represents a survivor. Record timestamps, validity/status and calibration metadata.
- CI build success confirms compilation only; it does not confirm electrical compatibility, sensor communication, ranging accuracy or survivor-detection performance.
- See [sensor library integration notes](../docs/hardware/sensor-libraries.md), [hardware review report](../docs/hardware/review-report-for-teacher.md), and [software status](../docs/software/status.md).


## Bare DWM3000 module

Because the project uses the bare DWM3000 module (not the DWM3000EVB), carrier-PCB, power, RF-layout and SPI/GPIO requirements must be resolved before firmware activation. See [bare-module integration plan](../docs/hardware/dwm3000-bare-module-integration.md).

## Driver implementation

The current guarded acquisition layer and its hardware enablement/verification gates are documented in [sensor-driver implementation](../docs/hardware/sensor-driver-implementation.md). The safe default leaves GPIO-dependent drivers disabled until the physical board revision and wiring are verified. DWM3000 and XVF3800 deliberately report `not_integrated`; they must not be treated as operational.


## Two-board roles

- **Card 1 gateway:** bounded UART frame parser, JSON validation, optional TLS MQTT telemetry publishing. MQTT is disabled by default and requires a local, ignored include/ares_secrets.h with Wi-Fi credentials, unique broker credentials, host name and trusted root CA. It does not subscribe to commands.
- **Card 2 sensor node:** current sensor-acquisition scaffold. If verified ARES_LINK_RX_PIN and ARES_LINK_TX_PIN raw GPIO values are set in the local ignored include/ares_board_config.h, telemetry uses UART2 at 115200 baud; otherwise it remains on USB serial for bench bring-up.
- **Build wrapper:** run python firmware/build_firmware.py --role both. This compiles two binaries and writes SHA-256 metadata; it does not flash the boards. The MCU runs compiled C/C++; Go and Python remain host-side programs rather than being translated into MCU runtime code.
