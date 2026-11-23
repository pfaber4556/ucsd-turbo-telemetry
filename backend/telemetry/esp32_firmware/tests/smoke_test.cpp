#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include "sensor_config.hpp"
#include "simulated_sensors.hpp"
#include "sensors.hpp"
#include "telemetry.hpp"
#include "unconfigured_sensors.hpp"

class BrokenRpm final : public turbine::RpmSensor {
public:
    turbine::Reading readRpm(std::uint64_t) override { return {-1.0, true}; }
};
class BrokenTemp final : public turbine::TemperatureSensor {
public:
    turbine::Reading readCelsius(std::uint64_t) override {
        return {std::numeric_limits<double>::quiet_NaN(), true};
    }
};
class BrokenPressure final : public turbine::PressureSensor {
public:
    turbine::Reading readGaugePsi(std::uint64_t) override { return {0.0, false}; }
};

int main() {
    turbine::SimulatedRpmSensor rpm;
    turbine::SimulatedTemperatureSensor t1(turbine::Station::One);
    turbine::SimulatedTemperatureSensor t2(turbine::Station::Two);
    turbine::SimulatedPressureSensor p1(turbine::Station::One);
    turbine::SimulatedPressureSensor p2(turbine::Station::Two);
    turbine::SensorHub hub(rpm, t1, p1, t2, p2);

    char line[128];
    for (std::uint64_t i = 0; i < 600; ++i) {
        const auto sample = hub.read(i * 100);
        assert(sample.timestamp_ms == i * 100);
        assert(sample.fault_code == 0);
        const auto n = turbine::format_protocol_v1_1(sample, line, sizeof(line));
        assert(n != 0 && line[n - 1] == '\n');
        assert(std::strlen(line) == n);
        assert(std::count(line, line + n, ',') == 6);
        unsigned long long ms = 0;
        unsigned int rev = 0, fault = 99;
        double a, b, c, d;
        int read = std::sscanf(line, "%llu,%u,%lf,%lf,%lf,%lf,%u",
                               &ms, &rev, &a, &b, &c, &d, &fault);
        assert(read == 7 && ms == i * 100 && fault == 0);
        assert(std::isfinite(a) && std::isfinite(b) && std::isfinite(c) && std::isfinite(d));
    }
    // Failure of one sensor type must be represented by the existing fault bit.
    BrokenRpm broken_rpm;
    BrokenTemp broken_temp;
    BrokenPressure broken_pressure;
    turbine::SensorHub bad(broken_rpm, broken_temp, broken_pressure, t2, p2);
    const auto bad_sample = bad.read(100);
    assert(bad_sample.fault_code == 7);
    assert(bad_sample.rpm == 0 && bad_sample.station1.temperature_c == 0 &&
           bad_sample.station1.static_pressure_psi == 0);
    assert(turbine::format_protocol_v1_1(bad_sample, line, sizeof(line)) > 0);
    assert(std::count(line, line + std::strlen(line), ',') == 6);
    // Hardware mode may not claim success when nothing is attached.
    turbine::UnconfiguredRpmSensor missing_rpm;
    turbine::UnconfiguredTemperatureSensor missing_temp1, missing_temp2;
    turbine::UnconfiguredPressureSensor missing_pressure1, missing_pressure2;
    turbine::SensorHub missing(missing_rpm, missing_temp1, missing_pressure1,
                               missing_temp2, missing_pressure2);
    const auto missing_sample = missing.read(1234);
    assert(missing_sample.fault_code == 7); // missing temp+pressure+RPM
    assert(missing_sample.rpm == 0);
    assert(missing_sample.station1.temperature_c == 0);
    assert(missing_sample.station2.temperature_c == 0);
    assert(missing_sample.station1.static_pressure_psi == 0);
    assert(missing_sample.station2.static_pressure_psi == 0);
    assert(turbine::format_protocol_v1_1(missing_sample, line, sizeof(line)) > 0);
    assert(std::count(line, line + std::strlen(line), ',') == 6);
    std::puts("PASS: 600 simulated seven-field CSV records, 100 ms increments, fault-bit tests, unconfigured hardware-mode fail-closed test");
}
