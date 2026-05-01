import { useEffect, useRef } from "react";
import { useWasm } from "../WasmProvider.jsx";

function resolveMembraneReader(Module) {
  if (typeof Module?.getMembranePotential === "function") {
    return Module.getMembranePotential.bind(Module);
  }

  if (typeof Module?.neuralBridge?.getMembranePotential === "function") {
    return Module.neuralBridge.getMembranePotential.bind(Module.neuralBridge);
  }

  return null;
}

export default function SelectedNeuronPanel({ targetIndex = 0 }) {
  const { Module, isReady } = useWasm();
  const voltageRef = useRef(null);
  const frameRef = useRef(0);

  useEffect(() => {
    if (!isReady || !Module) {
      return undefined;
    }

    const getMembranePotential = resolveMembraneReader(Module);

    function updateVoltageReadout() {
      if (voltageRef.current && getMembranePotential) {
        const membranePotential = getMembranePotential(targetIndex);
        voltageRef.current.innerText = `${membranePotential.toFixed(2)} mV`;
      }

      frameRef.current = window.requestAnimationFrame(updateVoltageReadout);
    }

    frameRef.current = window.requestAnimationFrame(updateVoltageReadout);

    return () => {
      window.cancelAnimationFrame(frameRef.current);
    };
  }, [Module, isReady, targetIndex]);

  return (
    <section
      className="absolute left-6 bottom-6 pointer-events-auto w-[min(320px,calc(100vw-48px))] rounded-lg border border-[rgba(229,229,234,0.2)] bg-[rgba(0,0,0,0.7)] p-5 text-white shadow-2xl backdrop-blur-md"
      aria-label="Selected neuron membrane potential"
    >
      <p className="mb-2 text-xs font-medium text-[rgba(235,235,245,0.58)]">
        Selected neuron
      </p>
      <div className="flex items-end justify-between gap-4">
        <div>
          <h2 className="m-0 text-[22px] font-semibold leading-none tracking-normal">
            Node {targetIndex}
          </h2>
          <p className="mt-2 text-sm text-[rgba(235,235,245,0.62)]">
            Membrane potential
          </p>
        </div>
        <output
          ref={voltageRef}
          className="min-w-[108px] text-right text-[26px] font-semibold leading-none tracking-normal"
        >
          --.-- mV
        </output>
      </div>
    </section>
  );
}
