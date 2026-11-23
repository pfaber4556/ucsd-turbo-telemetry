#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include "telemetry.hpp"

namespace turbine {

// An individual reading and its validity (a missing sensor must not be
// silently interpreted as a healthy zero reading).
struct Reading {
    double value = 0.0;
    bool valid = false;
};

// Each physical or simulated driver implements one of these interfaces.
// Readings must already be in the listed engineering units.
class RpmSensor {
public:
    virtual ~RpmSensor() = default;
    virtual Reading readRpm(std::uint64_t timestamp_ms) = 0;
};

class TemperatureSensor {
public:
    virtual ~TemperatureSensor() = default;
    virtual Reading readCelsius(std::uint64_t timestamp_ms) = 0;
};

class PressureSensor {
public:
    virtual ~PressureSensor() = default;
    virtual Reading readGaugePsi(std::uint64_t timestamp_ms) = 0;
};

// The collector does not care whether sensors are simulated, I2C, SPI,
// pulse-counting, or conditioned analog drivers. The wire format stays stable.
class SensorHub {
public:
    SensorHub(RpmSensor& rpm, TemperatureSensor& t1, PressureSensor& p1,
              TemperatureSensor& t2, PressureSensor& p2)
        : rpm_(rpm), t1_(t1), p1_(p1), t2_(t2), p2_(p2) {}

    Sample read(std::uint64_t timestamp_ms) {
        Sample sample{};
        sample.timestamp_ms = timestamp_ms;
        sample.fault_code = 0;

        const Reading rpm = rpm_.readRpm(timestamp_ms);
        if (rpm.valid && std::isfinite(rpm.value) && rpm.value >= 0.0 &&
            rpm.value <= static_cast<double>(std::numeric_limits<std::uint32_t>::max())) {
            sample.rpm = static_cast<std::uint32_t>(std::llround(rpm.value));
        } else {
            sample.rpm = 0;
            sample.fault_code |= kRpmFault;
        }

        sample.station1.temperature_c = validatedTemperature(
            t1_.readCelsius(timestamp_ms), sample.fault_code);
        sample.station1.static_pressure_psi = validatedPressure(
            p1_.readGaugePsi(timestamp_ms), sample.fault_code);
        sample.station2.temperature_c = validatedTemperature(
            t2_.readCelsius(timestamp_ms), sample.fault_code);
        sample.station2.static_pressure_psi = validatedPressure(
            p2_.readGaugePsi(timestamp_ms), sample.fault_code);
        return sample;
    }

private:
    static float validatedTemperature(Reading r, std::uint8_t& fault) {
        const float value = static_cast<float>(r.value);
        if (!r.valid || !std::isfinite(r.value) || !std::isfinite(value)) {
            fault |= kTemperatureFault;
            return 0.0f; // Placeholder only; fault bit marks invalid.
        }
        return value;
    }

    static float validatedPressure(Reading r, std::uint8_t& fault) {
        const float value = static_cast<float>(r.value);
        if (!r.valid || !std::isfinite(r.value) || !std::isfinite(value)) {
            fault |= kPressureFault;
            return 0.0f; // Placeholder only; fault bit marks invalid.
        }
        return value;
    }

    RpmSensor& rpm_;
    TemperatureSensor& t1_;
    PressureSensor& p1_;
    TemperatureSensor& t2_;
    PressureSensor& p2_;
};

} // namespace turbine
