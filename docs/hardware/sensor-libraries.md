# ARES Sensor Library Registry

This registry records the software source for each actual BOM component. A library is not marked integrated until its license, revision, target, wiring, and hardware test are verified.

## Integration status

| Component | Candidate source / approach | Status | Integration constraint |
|---|---|---|---|
| FLIR Lepton 3.5 | [NachtRaveVL/Lepton-FLiR-Arduino](https://github.com/NachtRaveVL/Lepton-FLiR-Arduino) | Candidate only | Older Arduino library; validate Lepton 3.5 radiometry, VoSPI timing, breakout requirements, and ESP32 compatibility before adding to firmware dependencies. |
| Qorvo DWM3000 | [Makerfabs ESP32 UWB DW3000](https://github.com/Makerfabs/Makerfabs-ESP32-UWB-DW3000) | Candidate only | Module/board compatibility, SPI pins, IRQ/reset handling, regulatory region, and ranging accuracy must be tested. Do not substitute a DW1000-only library. |
| XMOS XVF3800 + 4 PDM MEMS mics | [ReSpeaker XVF3800 USB 4-Mic Array](https://github.com/respeaker/reSpeaker_XVF3800_USB_4MIC_ARRAY) and [Seeed documentation](https://wiki.seeedstudio.com/respeaker_xvf3800_with_xiao/) | Host-side integration | XVF3800 performs audio processing on its own hardware. Choose the actual board transport (USB/I²S/etc.) and integrate its host interface; do not treat the four PDM microphones as directly wired to the Deneyap board. |
| Garmin LIDAR-Lite v3 | [Garmin LIDAR-Lite v3 Arduino library source reference](https://github.com/nathancy/Arduino-Robotics/tree/master/libraries/LIDARLite_v3_Arduino_Library-master) | Candidate only | Confirm repository availability, API, license, bus voltage, and ESP32/PlatformIO build compatibility. This is not a VL53L1X sensor. |
| Geospace GS-One LF | Manufacturer documentation and a project-specific acquisition driver | No generic library selected | Identify exact model/output and electrical characteristics. Requires a suitable low-noise analog front end and ADC; never connect an unknown geophone output directly to an MCU pin. |
| 74LVC2G17 | No software library | Hardware component | Schmitt-trigger buffer; verify supply and input/output voltage compatibility in the schematic. Not a bidirectional level shifter. |
| SD card (32 GB) | PlatformIO/Arduino ESP32 SD or SD_MMC API, depending on wiring | Platform API candidate | Verify card voltage, interface/pins, filesystem, sustained write rate, and power-loss behavior. |
| Deneyap Kart V2 | [Deneyap Kart documentation](https://docs.deneyap.org/) and ESP32 Arduino/PlatformIO platform | Board support | Confirm the exact board definition and framework against the installed board revision. |

## Why the firmware dependency list is not changed automatically

The current firmware/platformio.ini contains MLX90640 and VL53L1X dependencies, which do not match the currently stated FLIR Lepton 3.5 and Garmin LIDAR-Lite v3 inventory. They are not silently retained as drivers for different hardware, nor are unverified libraries added to the build.

After confirming exact breakout/board revisions, each candidate must pass:
1. License and upstream revision review.
2. Clean PlatformIO dependency resolution and compilation.
3. Bus-level bring-up test with timeout/error handling.
4. Sensor output validation against a reference or controlled test.
5. Timestamped telemetry and disconnect/recovery test.

Record the tested board revision, library commit/version, wiring, test procedure, and result here before marking a driver integrated.
