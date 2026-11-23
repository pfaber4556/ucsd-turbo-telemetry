#pragma once

#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cstdio>
#include <cstring>

namespace turbine {

// Bitmask preserved from original team protocol v1.0.
inline constexpr std::uint8_t kTemperatureFault = 1;
inline constexpr std::uint8_t kPressureFault = 2;
inline constexpr std::uint8_t kRpmFault = 4;

struct StationReading {
    float temperature_c;
    float static_pressure_psi; // Gauge pressure; negative is permitted.
};

struct Sample {
    std::uint64_t timestamp_ms;
    std::uint32_t rpm;
    StationReading station1;
    StationReading station2;
    std::uint8_t fault_code;
};

// Wire protocol v1.1/v1.2: 7 numeric fields, newline, no CSV header.
// It is intentionally unchanged in the modular firmware v1.3.
// Returns 0 if buffer space is insufficient or formatting fails.
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

} // namespace turbine
