import math
import time

start_time = time.monotonic()


def get_sensor_data():
    elapsed = time.monotonic() - start_time

    return {
        "timestamp_ms": int(elapsed * 1000),
        "rpm": round(1200 + 300 * math.sin(elapsed)),
        "temperature_c": round(
            25 + 5 * math.sin(elapsed / 3), 2
        ),
        "pressure_psi": round(
            1.2 + 0.3 * math.sin(elapsed / 2), 2
        ),
        "fault_code": 0,
    }

