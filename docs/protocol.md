# Turbine Telemetry Communication Protocol

**Project:** UCSD Turbine Telemetry System  
**Version:** 1.0  
**Status:** Initial Development

## 1. Overview

This document defines the communication protocol between the ESP32 microcontroller and the Python ground station.

The system collects turbine sensor data, transmits it to a computer, validates the measurements, and displays them on a real-time dashboard.

The initial version will use simulated sensor data before integrating physical hardware.

## 2. System Architecture

The system consists of three components:

**1. ESP32 Firmware (Embedded Systems Engineer)**
- Reads temperature, pressure, and RPM sensors.
- Converts sensor readings into engineering units.
- Transmits telemetry over USB serial.
- Reports sensor faults.

**2. Python Data Pipeline (Data & Analytics Engineer)**
- Receives telemetry from the ESP32.
- Parses and validates incoming packets.
- Saves valid measurements to CSV files.
- Provides data to the dashboard.

**3. Ground Station Dashboard (Software Lead)**
- Displays live temperature, pressure, and RPM readings.
- Plots measurements over time.
- Displays connection and sensor status.
- Shows visual alerts when measurements exceed configured limits.

## 3. Communication Interface

| Parameter | Specification |
|---|---|
| Communication | USB Serial |
| Baud Rate | 115200 |
| Data Format | CSV |
| Encoding | UTF-8 |
| Transmission Frequency | 10 Hz (initial target) |
| Line Ending | `\n` |
| Direction | ESP32 → Python |

Each telemetry packet must be transmitted on a single line.

The baud rate and sampling frequency may be increased after initial testing.

## 4. Telemetry Packet Format

Each packet contains five fields in the following order:

`timestamp_ms,rpm,temperature_c,pressure_psi,fault_code`

| Field | Type | Description | Unit |
|---|---|---|---|
| timestamp_ms | Integer | Milliseconds since ESP32 startup | ms |
| rpm | Integer | Shaft rotational speed | RPM |
| temperature_c | Float | Sensor temperature | °C |
| pressure_psi | Float | Gauge pressure | PSI |
| fault_code | Integer | Sensor/system fault status | N/A |

All numeric fields are required.

### Example telemetry packets

```csv
1000,1200,25.5,1.2,0
1100,1250,25.8,1.3,0
1200,1300,26.1,1.4,0
```

The ESP32 must not transmit a header row. Field names are defined in this document.

Values shown above are simulated examples.

## 5. Fault Codes

For the initial prototype, use the following fault codes:

| Code | Meaning |
|---|---|
| 0 | No reported fault |
| 1 | Temperature sensor fault |
| 2 | Pressure sensor fault |
| 4 | RPM sensor fault |

Fault codes use a bitmask, allowing multiple faults to be represented simultaneously.

For example, a fault code of `3` indicates both temperature and pressure sensor faults.

Fault code `0` means no fault was reported; it does not guarantee that the system is safe.

## 6. Data Validation

The Python data pipeline must:

1. Confirm that each packet contains exactly five fields.
2. Verify that fields contain valid numeric values.
3. Reject malformed or incomplete packets.
4. Reject negative timestamps and negative RPM values.
5. Preserve the fault code for logging and display.
6. Record validation errors for debugging.

For the initial prototype, negative gauge pressure is allowed because some pressure sensors can measure vacuum relative to atmospheric pressure.

Invalid packets must not crash the application.

## 7. Data Logging

Valid telemetry packets should be saved to timestamped CSV files.

Example filename:

`test_run_2026-11-22_143000.csv`

The saved CSV file must contain the following header:

```csv
timestamp_ms,rpm,temperature_c,pressure_psi,fault_code
```

The logger should preserve measurement timestamps and record readings in their received order.

## 8. Dashboard Requirements

The Python dashboard must display:

- Current shaft RPM
- Current temperature in °C
- Current pressure in PSI
- Sensor fault status
- Connection status
- Real-time measurement graphs

The dashboard should clearly indicate whether readings are simulated or from physical sensors.

## 9. Initial Testing Requirements

Before integrating physical sensors, the team must verify that:

- Simulated packets follow the specified format.
- The Python parser accepts valid packets.
- Invalid packets are rejected safely.
- The logger saves valid readings correctly.
- The dashboard displays simulated readings.
- The system handles disconnected devices without crashing.

## 10. Safety and Limitations

This initial telemetry system is intended for educational development and monitoring only.

It must not be used as the sole protective system for a functioning gas turbine.

Engine shutdown, overspeed protection, overtemperature protection, and other safety-critical functions require independently engineered protections and qualified engineering review.

## 11. Future Improvements

Potential future versions may include:

- Higher sampling rates
- Packet checksums
- Packet sequence numbers
- Vibration monitoring
- Additional temperature and pressure sensors
- Binary telemetry protocols
- Live anomaly detection
- Automated test reports
- Engine simulation integration

Protocol changes must be documented and communicated to all software members before implementation.
