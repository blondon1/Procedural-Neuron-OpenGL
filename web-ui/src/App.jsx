import { useState } from "react";
import { WasmProvider, useWasm } from "./WasmProvider.jsx";

function FloatingPanel() {
  const { isReady } = useWasm();
  const [sampleCount, setSampleCount] = useState(0);

  return (
    <section className="floating-panel pointer-events-auto" aria-label="Neural telemetry">
      <div>
        <p className="panel-kicker">Runtime</p>
        <h1 className="panel-title">{isReady ? "Online" : "Starting"}</h1>
      </div>
      <button
        className="panel-button"
        type="button"
        onClick={() => setSampleCount((current) => current + 1)}
      >
        Sample {sampleCount}
      </button>
    </section>
  );
}

export default function App() {
  return (
    <main className="neural-stage bg-black text-white">
      <canvas id="canvas" className="neural-canvas" />

      <WasmProvider>
        <div className="ui-overlay pointer-events-none">
          <FloatingPanel />
        </div>
      </WasmProvider>
    </main>
  );
}
