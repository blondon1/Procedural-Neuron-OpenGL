import { WasmProvider } from "./WasmProvider.jsx";
import SelectedNeuronPanel from "./components/SelectedNeuronPanel.jsx";
import TourController from "./components/TourController.jsx";

export default function App() {
  return (
    <main className="neural-stage bg-black text-white">
      <canvas id="canvas" className="neural-canvas" />

      <WasmProvider>
        <div className="ui-overlay pointer-events-none">
          <SelectedNeuronPanel targetIndex={0} />
          <TourController />
        </div>
      </WasmProvider>
    </main>
  );
}
