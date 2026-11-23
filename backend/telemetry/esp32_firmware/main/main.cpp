#include <cstdio>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "telemetry.hpp"

// UCSD Turbine Ground Station: simulation-only ESP32 telemetry transmitter.
// On a typical ESP32 DevKit, the USB cable uses a USB-to-UART console bridge.
// Native USB variants may require selecting a different ESP-IDF console.
extern "C" void app_main(void) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    TickType_t last_wake = xTaskGetTickCount();
    char packet[128];

    while (true) {
        const std::uint64_t uptime_ms =
            static_cast<std::uint64_t>(esp_timer_get_time()) / 1000ULL;
        const turbine::Sample sample = turbine::simulate(uptime_ms);
        const std::size_t length =
            turbine::format_protocol_v1_1(sample, packet, sizeof(packet));

        // The telemetry application emits one numeric CSV line, no header.
        if (length != 0) {
            std::fwrite(packet, 1, length, stdout);
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(turbine::kTransmitPeriodMs));
    }
}
