from fastapi import FastAPI

app = FastAPI(title="UCSD Turbine Telemetry API")

@app.get("/")
def home():
    return {"message": "Turbine Telemetry API running"}

@app.get("/health")
def health():
    return {"status": "ok"}
