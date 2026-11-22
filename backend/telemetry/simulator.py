import math
import time

def get_sensor_data():
    t = time.monotonic()

    return {
        "timestamp_ms": int(t * 1000),
        "rpm": round(1200 + 300 * math.sin(t / 2)),
        "temperature_c": round(25 + 5 * math.sin(t / 3), 2),
        "pressure_psi": round(1.2 + 0.3 * math.sin(t / 2), 2),
        "fault_code": 0,
    }
