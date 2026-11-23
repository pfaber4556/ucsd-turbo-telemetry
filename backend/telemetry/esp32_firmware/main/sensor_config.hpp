#pragma once
#include <cstdint>

// Choose firmware data source at compile time:
//   0 = HARDWARE mode (default): requires actual sensor drivers; the supplied
//       unconfigured placeholders report faults and NEVER synthesize readings.
//   1 = SIMULATION mode: useful for serial/parser testing only.
// Real driver classes must be added before hardware mode yields valid data.
#ifndef TURBINE_USE_SIMULATION
#define TURBINE_USE_SIMULATION 1
#endif

namespace turbine {

// Configuration for the reusable data acquisition application.
// These settings are unrelated to turbine operation or safety limits.
inline constexpr std::uint32_t kTransmitPeriodMs = 100; // 10 Hz
inline constexpr std::uint32_t kBaudRate = 115200;

// Station labels are metadata for experiments. CSV columns remain fixed.
// Coordinate actual locations with the experiment team and log them separately.
inline constexpr const char* kStation1Label = "Station 1 (location TBD)";
inline constexpr const char* kStation2Label = "Station 2 (location TBD)";

// Arbitrary demonstration values, NOT equipment operating specifications.
inline constexpr double kDemoPeakRpm = 4800.0;
inline constexpr double kDemoAmbientC = 24.5;

} // namespace turbine
