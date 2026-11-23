#pragma once

#include <cstdint>
#include "sensors.hpp"

namespace turbine {

// HARDWARE MODE PLACEHOLDERS.
// These classes deliberately return invalid readings until actual, validated
// hardware drivers are installed. They do NOT fabricate sensor measurements.
// Replace each instance with a driver appropriate to the identified hardware.

class UnconfiguredRpmSensor final : public RpmSensor {
public:
    Reading readRpm(std::uint64_t /*timestamp_ms*/) override {
        return {0.0, false};
    }
};

class UnconfiguredTemperatureSensor final : public TemperatureSensor {
public:
    Reading readCelsius(std::uint64_t /*timestamp_ms*/) override {
        return {0.0, false};
    }
};

class UnconfiguredPressureSensor final : public PressureSensor {
public:
    Reading readGaugePsi(std::uint64_t /*timestamp_ms*/) override {
        return {0.0, false};
    }
};

} // namespace turbine
