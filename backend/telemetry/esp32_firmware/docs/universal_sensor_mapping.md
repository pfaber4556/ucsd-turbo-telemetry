# Universal Sensor Mapping — v1.4

| CSV field | Sensor interface | Hardware-required mode (default) | Simulation mode | Fault bit |
|---|---|---|---|---|
| rpm | `RpmSensor::readRpm` | UnconfiguredRpmSensor (invalid) | SimulatedRpmSensor | 4 |
| temperature_1_c | `TemperatureSensor::readCelsius` | UnconfiguredTemperatureSensor (invalid) | SimulatedTemperatureSensor(One) | 1 |
| pressure_1_psi | `PressureSensor::readGaugePsi` | UnconfiguredPressureSensor (invalid) | SimulatedPressureSensor(One) | 2 |
| temperature_2_c | `TemperatureSensor::readCelsius` | UnconfiguredTemperatureSensor (invalid) | SimulatedTemperatureSensor(Two) | 1 |
| pressure_2_psi | `PressureSensor::readGaugePsi` | UnconfiguredPressureSensor (invalid) | SimulatedPressureSensor(Two) | 2 |

The seven-field CSV protocol stays unchanged. The **default hardware-required mode contains no physical acquisition drivers yet** and declares all five sensor readings invalid. Source mode must be tracked separately in the Python ground station run metadata.

Unconnected data uses zero placeholders accompanied by fault bits, not actual zero measurements. Sensor calibration, hardware compatibility, and measuring location must be approved before connecting the sensors to operating equipment. The ESP32 performs monitoring only.
