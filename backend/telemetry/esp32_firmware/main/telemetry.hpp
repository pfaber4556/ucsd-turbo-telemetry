#pragma once

<<<<<<< HEAD
=======
#include <algorithm>
#include <cmath>
>>>>>>> 9287a594362a91a2fb066ba87dd6c824ab5e311f
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cstdio>
#include <cstring>

namespace turbine {

<<<<<<< HEAD
// Bitmask preserved from original team protocol v1.0.
=======
// UCSD Turbine Ground Station: simulated five-channel serial telemetry.
// Protocol v1.2 retains the seven CSV columns introduced in proposed v1.1.
// The numeric signals below are synthetic test data, NOT a turbine model.
inline constexpr std::uint32_t kTransmitPeriodMs = 100;  // 10 Hz
inline constexpr std::uint32_t kBaudRate = 115200;

// Aggregate fault bits retained from the original protocol.
// These do not identify which of the two station sensors has failed.
>>>>>>> 9287a594362a91a2fb066ba87dd6c824ab5e311f
inline constexpr std::uint8_t kTemperatureFault = 1;
inline constexpr std::uint8_t kPressureFault = 2;
inline constexpr std::uint8_t kRpmFault = 4;

<<<<<<< HEAD
struct StationReading {
    float temperature_c;
    float static_pressure_psi; // Gauge pressure; negative is permitted.
=======
// Adjust these DISPLAY-TEST values as needed for the dashboard.
// They are not calibrated sensor ranges or turbine operating limits.
inline constexpr double kSimulatedPeakRpm = 4800.0;
inline constexpr double kSimulatedAmbientC = 24.5;

struct StationReading {
    float temperature_c;
    float static_pressure_psi;  // Gauge pressure relative to ambient.
>>>>>>> 9287a594362a91a2fb066ba87dd6c824ab5e311f
};

struct Sample {
    std::uint64_t timestamp_ms;
    std::uint32_t rpm;
<<<<<<< HEAD
    StationReading station1;
    StationReading station2;
    std::uint8_t fault_code;
};

// Wire protocol v1.1/v1.2: 7 numeric fields, newline, no CSV header.
// It is intentionally unchanged in the modular firmware v1.3.
// Returns 0 if buffer space is insufficient or formatting fails.
=======
    StationReading station1;  // Measurement station 1 (placement TBD).
    StationReading station2;  // Measurement station 2 (placement TBD).
    std::uint8_t fault_code;
};

// Repeating 60-second demonstration cycle:
// 0-3 s idle, 3-13 s simulated RPM ramp-up, 13-43 s steady,
// 43-53 s ramp-down, 53-60 s idle.
// NO actuator commands or interaction with a physical turbine.
inline Sample simulate(std::uint64_t uptime_ms) {
    const double seconds = static_cast<double>(uptime_ms) / 1000.0;
    const double phase = std::fmod(seconds, 60.0);

    double command = 0.0;
    if (phase >= 3.0 && phase < 13.0) {
        command = (phase - 3.0) / 10.0;
    } else if (phase >= 13.0 && phase < 43.0) {
        command = 1.0;
    } else if (phase >= 43.0 && phase < 53.0) {
        command = (53.0 - phase) / 10.0;
    }

    // Small reproducible variations help test live plots and CSV logging.
    const double jitter = command > 0.0
        ? 0.006 * std::sin(seconds * 2.3) + 0.003 * std::sin(seconds * 5.1)
        : 0.0;
    const double level = std::clamp(command + jitter, 0.0, 1.0);
    const double ambient_c = kSimulatedAmbientC +
        0.10 * std::sin(seconds / 20.0);

    Sample sample{};
    sample.timestamp_ms = uptime_ms;
    sample.rpm = static_cast<std::uint32_t>(
        std::lround(kSimulatedPeakRpm * level));

    // Two INDEPENDENT demo traces. Relative magnitudes have no physical
    // significance: not a claim of pressure gain, pressure drop, heating,
    // cooling, or station geometry for any turbine type.
    sample.station1.temperature_c = static_cast<float>(
        ambient_c + 1.2 * level + 0.06 * level * std::sin(seconds * 0.7));
    sample.station1.static_pressure_psi = static_cast<float>(
        level * (0.22 + 0.01 * std::sin(seconds * 0.9)));

    sample.station2.temperature_c = static_cast<float>(
        ambient_c + 2.1 * level + 0.05 * level * std::sin(seconds * 0.5));
    sample.station2.static_pressure_psi = level == 0.0
        ? 0.0f
        : static_cast<float>(level * (-0.09 + 0.01 * std::sin(seconds * 1.1)));

    // No injected faults. 0 means no fault reported, not 'safe'.
    sample.fault_code = 0;
    return sample;
}

// Seven fields, always in this exact order, with no header:
// timestamp_ms,rpm,temperature_1_c,pressure_1_psi,
// temperature_2_c,pressure_2_psi,fault_code
// Returns 0 on error and never writes a partial record to the output buffer.
>>>>>>> 9287a594362a91a2fb066ba87dd6c824ab5e311f
inline std::size_t format_protocol_v1_1(const Sample& sample,
                                        char* buffer,
                                        std::size_t capacity) {
    if (buffer == nullptr || capacity == 0) return 0;

    char scratch[128];
    const int n = std::snprintf(scratch, sizeof(scratch),
        "%" PRIu64 ",%" PRIu32 ",%.2f,%.3f,%.2f,%.3f,%u\n",
        sample.timestamp_ms,
        sample.rpm,
        static_cast<double>(sample.station1.temperature_c),
        static_cast<double>(sample.station1.static_pressure_psi),
        static_cast<double>(sample.station2.temperature_c),
        static_cast<double>(sample.station2.static_pressure_psi),
        static_cast<unsigned>(sample.fault_code));

    if (n < 0 || static_cast<std::size_t>(n) >= sizeof(scratch) ||
        static_cast<std::size_t>(n) >= capacity) return 0;

    std::memcpy(buffer, scratch, static_cast<std::size_t>(n) + 1);
    return static_cast<std::size_t>(n);
}

<<<<<<< HEAD
} // namespace turbine
=======
}  // namespace turbine
>>>>>>> 9287a594362a91a2fb066ba87dd6c824ab5e311f
