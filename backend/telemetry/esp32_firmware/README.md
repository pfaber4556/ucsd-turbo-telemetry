# UCSD Universal Sensor Telemetry — ESP-IDF C++ v1.4 (hardware-ready)

This is an incremental update of the successfully flashed **v1.3** ESP32 five-channel telemetry application. **It is hardware-ready, not a completed sensor integration.** The code cannot produce real RPM, temperature, or pressure measurements until the team supplies compatible physical sensors and their verified driver implementations.

## What is different from v1.3?

- `main/sensor_config.hpp` has a compile-time switch `TURBINE_USE_SIMULATION`.
- **Default `0`: hardware-required mode**, which never invents readings when drivers are missing.
- `main/unconfigured_sensors.hpp` contains deliberate *not-connected* sensor interfaces that report invalid readings.
- **Optional `1`: simulation mode**, for verifying the CSV pipeline with synthetic data.
- The `SensorHub`/CSV formatter and five-channel schema remain unchanged, preserving compatibility with the team's seven-field protocol proposal.

**Important:** Hardware mode currently reports a fault code of `7` with numeric placeholders of `0` for all five missing readings. These zeroes are NOT actual measurements. The ground station must mark a channel invalid whenever its corresponding fault bit is set. The three category bits cannot identify which of the two temperature/pressure stations failed; update the protocol collaboratively if per-channel fault detail is required.

## How to install in your already-working ESP-IDF project

Back up/commit the current project first, then copy the contents of this ZIP's `main/` directory into your existing project's `main/` directory. Keep the **existing** root `CMakeLists.txt` and `sdkconfig`; your existing `main/CMakeLists.txt` is also compatible. Never overwrite `sdkconfig` merely to apply these source changes.

ESP-IDF terminal:

```powershell
idf.py build
idf.py -p COM5 flash monitor
```

Replace `COM5` with the COM port assigned to your ELEGOO board. The actual ESP-IDF build and flash have **not** been tested in this environment, so verify them on your machine.

### Configure the source mode

Edit `main/sensor_config.hpp`:

```cpp
#define TURBINE_USE_SIMULATION 0 // default: hardware required (no real drivers installed yet)
```

For the original simulated CSV demonstration only, set it to `1` and rebuild/reflash. **This setting does not automatically make hardware drivers available.**

## CSV remains unchanged (7 fields, 115200 baud, nominal 10 Hz)

```
timestamp_ms,rpm,temperature_1_c,pressure_1_psi,temperature_2_c,pressure_2_psi,fault_code
```

When no real drivers are installed, a line resembles:

```
1234,0,0.00,0.000,0.00,0.000,7
```

`7 = 1 (temperature fault) | 2 (pressure fault) | 4 (RPM fault)`. The numeric placeholders must never be treated as measurements. The preexisting protocol requires numeric fields, so invalid channels currently use zeros plus fault bits, rather than NaN or empty fields. The Python ground station should validate and flag those values accordingly.

## What is needed to genuinely acquire physical sensor data?

To add a real driver for each channel, provide:

1. The **exact sensor model/part number and datasheet**, identifying its signal format (I²C, SPI, analog, conditioned pulse, etc.).
2. The corresponding **approved interface hardware and electrical limits**, including correct ESP32 logic/ADC compatibility.
3. Calibration and **units** (RPM, °C, gauge PSI). Do not confuse absolute pressure with gauge pressure.
4. How disconnected devices, invalid frames, and stale samples are detected. Do not return `valid=true` without verified, fresh acquisition.
5. The team's agreed channel location metadata (sensor-to-station mapping) and protocol version.

Then implement classes inheriting `RpmSensor`, `TemperatureSensor`, and `PressureSensor` as defined in `sensors.hpp`. In `main.cpp`, replace the five `Unconfigured...` instances with corresponding real driver objects. **Do not enable automatic fallback to simulation** when real sensors fail.

Initially validate the acquisition electronics with low-energy desktop/bench conditions, with no running turbine attached. Any installation on an operating high-speed, high-temperature gas turbine requires independent safety protections and qualified engineering review; this monitoring-only firmware is never an engine protective system.

## Desktop smoke test

```bash
g++ -std=c++17 -O2 -I main tests/smoke_test.cpp -o smoke_test
./smoke_test
```

This tests simulation formatting, invalid sensor handling and hardware-mode absence behavior. It does **not** test actual ESP32 peripheral drivers or sensor measurements.
