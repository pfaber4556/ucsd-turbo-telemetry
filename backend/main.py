import asyncio

from fastapi import FastAPI, WebSocket
from starlette.websockets import WebSocketDisconnect

from backend.telemetry.simulator import get_sensor_data

app = FastAPI(title="UCSD Turbine Telemetry API")


@app.get("/")
def home():
    return {"message": "Turbine Telemetry API running"}


@app.get("/health")
def health():
    return {"status": "ok"}


@app.websocket("/ws/telemetry")
async def telemetry_stream(websocket: WebSocket):
    await websocket.accept()

    try:
        while True:
            reading = get_sensor_data()

            await websocket.send_json(reading)

            # 10 readings per second
            await asyncio.sleep(0.1)

    except (WebSocketDisconnect, OSError):
        pass
