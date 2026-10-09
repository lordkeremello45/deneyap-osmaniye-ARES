# Research and decision log

Method: define acceptance criteria; collect manufacturer datasheets/reference designs; compare at least two candidates; eliminate electrical/mechanical mismatches and safety/regulatory risks; bench-test survivors; record date, setup, firmware and measurements; explain changes.

Open decisions:
- UART: test short 3.3 V wiring first; select buffer/transceiver only after cable and EMI requirements are measured.
- Sensors: models mentioned in conversation are candidates, not verified final selections.
- Power: size 5 V/3.3 V rails from measured total and peak current; keep motor power separate from logic rails.
- Propulsion: validate motor, ESC, propeller, battery, guard and total mass as one thrust budget.


## 2026-10-09 — research decisions

- **DWM3000 driver candidate:** use `br101/dw3000-decadriver-source` as the candidate baseline because it includes the Qorvo DW3xxx driver 08.02.02 and an ESP-IDF 5.1.4 port tested on DW3110/DWM3000. It is not yet integrated into the current Arduino-only PlatformIO environment. Before implementation: pin an upstream commit, inspect the Qorvo license, reconcile the physical Deneyap Kart V2 ESP32-S3 target, build the ESP-IDF + Arduino component combination, and verify the device ID before ranging.
- **XMOS data path:** prefer USB Audio Class 2.0 from a confirmed XVF3800 UA/USB assembly to the Windows companion host. Do not assume the DENEYAP board can be a USB host. If the inventory is only the XVF3800 chip and microphones, hardware/firmware design remains a prerequisite. I2S is a separate INT firmware path.
- **Propulsion release gate:** KV85 and KV150 are distinct variants on T-Motor's U8 Lite page; the KV150 thrust tables exist, but T-Motor's V-Link table also lists U8 Lite KV150 with 13S/MF24, while the product page gives separate 6S/12S data. Do not release a flight BOM until T-Motor confirms the exact SKU, voltage and propeller combination in writing.
- **No flight-controller recommendation is locked yet:** select FC, battery and power distribution only after rotor count/layout, all-up mass, payload, target endurance and propulsion configuration are established. A software/AI host must never replace the flight controller's stabilization or failsafe.
