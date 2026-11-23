
import { useEffect, useState } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";
import {
  Activity,
  Gauge,
  Thermometer,
  Wifi,
} from "lucide-react";
import "./App.css";

type Telemetry = {
  timestamp_ms: number;
  rpm: number;
  temperature_c: number;
  pressure_psi: number;
  fault_code: number;
};

export default function App() {
  const [history, setHistory] = useState<Telemetry[]>([]);
  const [recording, setRecording] = useState(false);

  useEffect(() => {
    const start = Date.now();

    const interval = setInterval(() => {
      const seconds = (Date.now() - start) / 1000;

      const reading: Telemetry = {
        timestamp_ms: Date.now() - start,
        rpm: Math.round(1200 + 300 * Math.sin(seconds)),
        temperature_c: 25 + 5 * Math.sin(seconds / 3),
        pressure_psi: 1.2 + 0.3 * Math.sin(seconds / 2),
        fault_code: 0,
      };

      setHistory((prev) => [...prev.slice(-99), reading]);
    }, 100);

    return () => clearInterval(interval);
  }, []);

  const latest = history[history.length - 1];

  const downloadCSV = () => {
    if (!history.length) return;

    const columns: (keyof Telemetry)[] = [
      "timestamp_ms",
      "rpm",
      "temperature_c",
      "pressure_psi",
      "fault_code",
    ];

    const csv = [
      columns.join(","),
      ...history.map((row) =>
        columns.map((column) => row[column]).join(",")
      ),
    ].join("\n");

    const url = URL.createObjectURL(
      new Blob([csv], { type: "text/csv" })
    );

    const link = document.createElement("a");
    link.href = url;
    link.download = "turbine_simulation.csv";
    link.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="dashboard">
      <header className="header">
        <div>
          <h1>UCSD Turbine Ground Station</h1>
          <p>Real-Time Turbomachinery Monitoring</p>
        </div>

        <div className="connection">
          <Wifi size={18} />
          <span>SIMULATION ACTIVE</span>
        </div>
      </header>

      <section className="metrics">
        <div className="metric-card">
          <div className="metric-title">
            <Gauge size={20} />
            <span>Shaft RPM</span>
          </div>
          <h2>{latest?.rpm.toLocaleString() ?? "0"}</h2>
          <p>Revolutions per minute</p>
        </div>

        <div className="metric-card">
          <div className="metric-title">
            <Thermometer size={20} />
            <span>Temperature</span>
          </div>
          <h2>
            {latest?.temperature_c.toFixed(1) ?? "0.0"} °C
          </h2>
          <p>Simulated temperature</p>
        </div>

        <div className="metric-card">
          <div className="metric-title">
            <Activity size={20} />
            <span>Pressure</span>
          </div>
          <h2>
            {latest?.pressure_psi.toFixed(2) ?? "0.00"} PSI
          </h2>
          <p>Gauge pressure</p>
        </div>
      </section>

      <section className="chart-grid">
        {([
          ["Shaft RPM", "rpm", "#38bdf8"],
          ["Temperature (°C)", "temperature_c", "#fb923c"],
          ["Pressure (PSI)", "pressure_psi", "#4ade80"],
        ] as const).map(([title, key, color]) => (
          <div className="chart-card" key={key}>
            <h3>{title}</h3>

            <div className="chart-container">
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={history}>
                  <CartesianGrid
                    stroke="#334155"
                    strokeDasharray="3 3"
                  />
                  <XAxis
                    dataKey="timestamp_ms"
                    stroke="#94a3b8"
                    tickFormatter={(value) =>
                      `${(value / 1000).toFixed(0)}s`
                    }
                  />
                  <YAxis stroke="#94a3b8" />
                  <Tooltip />
                  <Line
                    dataKey={key}
                    stroke={color}
                    strokeWidth={2}
                    dot={false}
                    isAnimationActive={false}
                  />
                </LineChart>
              </ResponsiveContainer>
            </div>
          </div>
        ))}
      </section>

      <section className="controls">
        <div>
          <h3>System Status</h3>
          <p>
            <span className="status-dot" />
            Simulation running — no physical engine connected
          </p>
        </div>

        <div className="buttons">
          <button
            onClick={() => setRecording(!recording)}
            className="record-btn"
          >
            {recording ? "Stop Recording" : "Start Recording"}
          </button>

          <button onClick={downloadCSV} className="export-btn">
            Export CSV
          </button>
        </div>
      </section>
    </div>
  );
}
