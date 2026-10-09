# ARES — Propulsion selection and flight-release gates

**Status:** research decision; not flight-approved  
**Reviewed:** 2026-10-09

## 1. Current decision

The T-Motor U8 Lite KV150 remains a candidate, but the propulsion subsystem is **blocked from flight use** until the manufacturer confirms the exact motor variant and matching set. This is not a claim that the system is flight-ready.

### What the official sources currently say

- [T-Motor U8 Lite KV150 product page](https://store.tmotor.com/product/u8-lite-u-efficiency-kv150.html) exposes separate KV85, KV100, KV150 and KV190 variants. The page's first/default specification table describes KV85, so it must not be read as the KV150 specification.
- The KV150 section lists different operating combinations, including a 6S table with G28×9.2 CF propeller and a 12S table with P22×6.6 CF propeller. Its table lists a peak current of about 29.3 A / 26.5 A and maximum power of about 712.8 W / 1272 W for the respective variants/configurations. These are manufacturer bench figures, not guaranteed in-air values.
- The same product page's matching guide names **G28×9.2 + Alpha 60A 6S** as a matching combination.
- T-Motor's [V-Link matching table](https://store.tmotor.com/product/v-link.html), however, lists **U8 LITE KV150 — 13S — MF24**. This conflicts with the product-page 6S/12S table and must be resolved by T-Motor for the exact SKU before selecting the final ESC, propeller and battery.

## 2. Temporary bench candidate — not a flight BOM

For a guarded static bench test only, the product-page pairing **U8 Lite KV150 + G28×9.2 CF propeller + Alpha 60A 6S ESC + a correctly specified 6S LiPo pack** is the clearest documented candidate. Do not purchase or energize this combination solely from this note: first confirm the motor label/SKU and obtain written manufacturer confirmation that the propeller, ESC and cell count apply to that exact KV150 revision.

Do not substitute the V-Link 13S/MF24 pairing into this setup or mix its parts with the 6S configuration. Propellers are high-energy hazards; tests require a proper thrust stand, rigid fixture, remote arming/kill, physical exclusion zone and appropriate protection.

## 3. Why battery capacity and flight controller are not finalized

Battery capacity depends on measured all-up mass, rotor count, target hover/endurance, reserve policy, current draw and pack discharge capability. A capacity recommendation without these inputs would be fabricated. After the manufacturer resolves the motor/ESC/propeller/voltage combination:

1. Define rotor count/layout, measured frame mass, sensor payload, wiring and landing/guard mass.
2. Establish target endurance and required reserve.
3. Obtain thrust/current/voltage/temperature data for the exact motor + ESC + propeller combination.
4. Size pack voltage to the confirmed motor/ESC combination; size continuous and burst current with engineering margin and account for wiring, connectors, fusing and voltage sag.
5. Calculate endurance from measured mission-profile current, not a marketing maximum-thrust figure.
6. Select a compatible power-distribution board, battery monitor, current/voltage sensing, fuse/anti-spark arrangement and connectors.
7. Select a dedicated ArduPilot/PX4-compatible flight controller only after rotor geometry, actuator count, receiver/telemetry interfaces, GNSS needs and failsafe requirements are known. The DENEYAP cards, Windows host, Gemma and MQTT are not substitutes for a flight controller.

The Pololu D24V50F5 is a 5 V buck regulator for logic/sensor loads; it must not power the propulsion system or be treated as an ESC.

## 4. Required evidence before the propulsion BOM can be approved

- [ ] Photograph/record the motor label and exact SKU/revision.
- [ ] Written T-Motor confirmation of KV150 motor voltage/cell count and the correct propeller + ESC pairing, resolving the 6S/12S vs 13S discrepancy.
- [ ] Exact ESC datasheet: voltage range, continuous/burst current at actual cooling, firmware/protocol, signal levels and low-voltage behavior.
- [ ] Exact propeller part number, material, diameter/pitch, hub/adapter and manufacturer-approved RPM limit.
- [ ] Battery part number, cell count, capacity, continuous/burst discharge rating, mass, connector and protection/monitoring plan.
- [ ] Measured all-up mass, rotor layout, thrust margin and expected per-motor hover thrust.
- [ ] Static thrust-stand data across the intended throttle range, recording voltage, current, thrust, ESC/motor temperature and vibration.
- [ ] Independent flight controller configured and tested for arming interlocks, RC/link loss, low battery, sensor faults and emergency disarm.
- [ ] Propeller guards/exclusion zone and staged tests: unpowered fit check → motor/ESC no-prop test → restrained low-power test → guarded static thrust test → tethered/controlled flight only after formal review.

## 5. Release rule

Until the manufacturer discrepancy and the evidence above are closed, label propulsion as **candidate / blocked**. No flight, no autonomous motor command, and no endurance or lift-capacity claim should be based on these preliminary tables.
