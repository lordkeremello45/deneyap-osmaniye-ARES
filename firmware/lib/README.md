# Local PlatformIO Libraries

Place ARES-owned, reusable PlatformIO libraries in this directory using one subdirectory per library. Each library should have a library.json (or library.properties where appropriate), src/, include/ if needed, license information, and a README with supported hardware and tests.

Third-party libraries should normally be pinned in firmware/platformio.ini under lib_deps after compatibility and license review. Do not copy arbitrary upstream code here without preserving its license and provenance.

See the Sensor Library Registry at docs/hardware/sensor-libraries.md for component-specific candidates and validation status.
