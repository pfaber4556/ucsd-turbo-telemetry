#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include "sensor_config.hpp"
#include "sensors.hpp"

namespace turbine {

// Synthetic, repeating 60-second demonstration trace. Not a gas turbine
// dynamics model. No GPIO, sensor hardware, actuator, or engine control code.
inline double demoSeconds(std::uint64_t ms) {
    return static_cast<double>(ms) / 1000.0;
}

inline double demoLevel(std::uint64_t ms) {
    const double seconds = demoSeconds(ms);
    const double phase = std::fmod(seconds, 60.0);
    double command = 0.0;
    if (phase >= 3.0 && phase < 13.0) {
        command = (phase - 3.0) / 10.0;
    } else if (phase >= 13.0 && phase < 43.0) {
        command = 1.0;
    } else if (phase >= 43.0 && phase < 53.0) {
        command = (53.0 - phase) / 10.0;
    }
    const double jitter = command > 0.0
        ? 0.006 * std::sin(seconds * 2.3) + 0.003 * std::sin(seconds * 5.1)
        : 0.0;
    return std::clamp(command + jitter, 0.0, 1.0);
}

inline double demoAmbientC(std::uint64_t ms) {
    return kDemoAmbientC + 0.10 * std::sin(demoSeconds(ms) / 20.0);
}

enum class Station { One, Two };

class SimulatedRpmSensor final : public RpmSensor {
public:
    Reading readRpm(std::uint64_t ms) override {
        return {std::round(kDemoPeakRpm * demoLevel(ms)), true};
    }
};

class SimulatedTemperatureSensor final : public TemperatureSensor {
public:
    explicit SimulatedTemperatureSensor(Station station) : station_(station) {}

    Reading readCelsius(std::uint64_t ms) override {
        const double level = demoLevel(ms);
        const double t = demoSeconds(ms);
        const double offset = station_ == Station::One
            ? 1.2 * level + 0.06 * level * std::sin(t * 0.7)
            : 2.1 * level + 0.05 * level * std::sin(t * 0.5);
        return {demoAmbientC(ms) + offset, true};
    }
private:
    Station station_;
};

class SimulatedPressureSensor final : public PressureSensor {
public:
    explicit SimulatedPressureSensor(Station station) : station_(station) {}

    Reading readGaugePsi(std::uint64_t ms) override {
        const double level = demoLevel(ms);
        const double t = demoSeconds(ms);
        const double value = station_ == Station::One
            ? level * (0.22 + 0.01 * std::sin(t * 0.9))
            : (level == 0.0 ? 0.0 : level * (-0.09 + 0.01 * std::sin(t * 1.1)));
        return {value, true};
    }
private:
    Station station_;
};

} // namespace turbine
