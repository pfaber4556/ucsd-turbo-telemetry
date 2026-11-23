# Hardware integration status — v1.4

| Channel | C++ interface | Current hardware-mode implementation | Valid actual readings? |
| --- | --- | --- | --- |
| RPM | `RpmSensor::readRpm` | UnconfiguredRpmSensor | No; fault bit 4 |
| Temperature 1 | `TemperatureSensor::readCelsius` | UnconfiguredTemperatureSensor | No; fault bit 1 |
| Pressure 1 | `PressureSensor::readGaugePsi` | UnconfiguredPressureSensor | No; fault bit 2 |
| Temperature 2 | `TemperatureSensor::readCelsius` | UnconfiguredTemperatureSensor | No; fault bit 1 |
| Pressure 2 | `PressureSensor::readGaugePsi` | UnconfiguredPressureSensor | No; fault bit 2 |

Firmware emits valid numeric CSV placeholders but declares **all measurements invalid** until specific, verified physical device drivers are installed. This prevents a disconnected sensor from being shown as healthy simulated data.

The Python ground station must distinguish simulation, valid measured data, and missing/invalid readings. Fault code 0 is not a turbine safety guarantee. Current protocol's temperature/pressure fault bits are grouped by sensor type, not per individual channel.
