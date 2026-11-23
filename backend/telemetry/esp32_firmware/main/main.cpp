#include <cstdio>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "sensor_config.hpp"
#include "sensors.hpp"
#include "telemetry.hpp"

#if TURBINE_USE_SIMULATION
#include "simulated_sensors.hpp"
#else
#include "unconfigured_sensors.hpp"
#endif

// UCSD Universal Turbine Telemetry — monitoring-only firmware.
// HARDWARE MODE IS THE DEFAULT. No synthetic fallback when sensors are missing.
// A real acquisition mode requires approved, device-specific sensor drivers.
// This program never controls, starts, or shuts down turbine equipment.
extern "C" void app_main(void) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

#if TURBINE_USE_SIMULATION
    turbine::SimulatedRpmSensor rpm;
    turbine::SimulatedTemperatureSensor temp1(turbine::Station::One);
    turbine::SimulatedPressureSensor pressure1(turbine::Station::One);
    turbine::SimulatedTemperatureSensor temp2(turbine::Station::Two);
    turbine::SimulatedPressureSensor pressure2(turbine::Station::Two);
#else
    // Replace these five declarations with real sensor drivers once the
    // sensor models, electrical interfaces, and unit conversions are defined.
    turbine::UnconfiguredRpmSensor rpm;
    turbine::UnconfiguredTemperatureSensor temp1;
    turbine::UnconfiguredPressureSensor pressure1;
    turbine::UnconfiguredTemperatureSensor temp2;
    turbine::UnconfiguredPressureSensor pressure2;
#endif

    turbine::SensorHub sensor_hub(rpm, temp1, pressure1, temp2, pressure2);

    TickType_t last_wake = xTaskGetTickCount();
    char packet[128];

    while (true) {
        const std::uint64_t uptime_ms =
            static_cast<std::uint64_t>(esp_timer_get_time()) / 1000ULL;
        const turbine::Sample sample = sensor_hub.read(uptime_ms);
        const std::size_t length =
            turbine::format_protocol_v1_1(sample, packet, sizeof(packet));
        if (length != 0) {
            std::fwrite(packet, 1, length, stdout);
        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(turbine::kTransmitPeriodMs));
    }
}
