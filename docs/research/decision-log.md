# Research and decision log

Method: define acceptance criteria; collect manufacturer datasheets/reference designs; compare at least two candidates; eliminate electrical/mechanical mismatches and safety/regulatory risks; bench-test survivors; record date, setup, firmware and measurements; explain changes.

Open decisions:
- UART: test short 3.3 V wiring first; select buffer/transceiver only after cable and EMI requirements are measured.
- Sensors: models mentioned in conversation are candidates, not verified final selections.
- Power: size 5 V/3.3 V rails from measured total and peak current; keep motor power separate from logic rails.
- Propulsion: validate motor, ESC, propeller, battery, guard and total mass as one thrust budget.
