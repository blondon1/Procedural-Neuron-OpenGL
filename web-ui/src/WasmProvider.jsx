import { createContext, useContext, useEffect, useMemo, useState } from "react";

const WasmContext = createContext({
  Module: null,
  isReady: false,
  error: null,
});

const DEFAULT_ENGINE_LOADER = "/wasm/Neurons2D.js";

function createBridgeManager(Module) {
  if (typeof Module?.NeuralNetworkManager !== "function") {
    return null;
  }

  try {
    const manager = new Module.NeuralNetworkManager();
    manager.initializeProceduralGraph(1000);
    return manager;
  } catch (bridgeError) {
    console.warn("Unable to create NeuralNetworkManager bridge.", bridgeError);
    return null;
  }
}

function attachPrimitiveBridge(Module, manager) {
  if (!manager) {
    return;
  }

  if (typeof Module.getMembranePotential !== "function") {
    Module.getMembranePotential = (index) => manager.getMembranePotential(index);
  }
  if (typeof Module.getPositionX !== "function") {
    Module.getPositionX = (index) => manager.getPositionX(index);
  }
  if (typeof Module.getPositionY !== "function") {
    Module.getPositionY = (index) => manager.getPositionY(index);
  }
  if (typeof Module.getNeuronCount !== "function") {
    Module.getNeuronCount = () => manager.getNeuronCount();
  }

  Module.neuralBridge = manager;
}

function loadCreateNeuralEngine(loaderUrl) {
  if (typeof window.createNeuralEngine === "function") {
    return Promise.resolve(window.createNeuralEngine);
  }

  const existingScript = document.querySelector(
    `script[data-neural-engine-loader="${loaderUrl}"]`,
  );

  if (existingScript) {
    return new Promise((resolve, reject) => {
      existingScript.addEventListener("load", () => {
        if (typeof window.createNeuralEngine === "function") {
          resolve(window.createNeuralEngine);
        } else {
          reject(new Error("createNeuralEngine was not found on the loaded script."));
        }
      });
      existingScript.addEventListener("error", () => {
        reject(new Error(`Failed to load ${loaderUrl}.`));
      });
    });
  }

  return new Promise((resolve, reject) => {
    const script = document.createElement("script");
    script.src = loaderUrl;
    script.async = true;
    script.dataset.neuralEngineLoader = loaderUrl;
    script.addEventListener("load", () => {
      if (typeof window.createNeuralEngine === "function") {
        resolve(window.createNeuralEngine);
      } else {
        reject(new Error("createNeuralEngine was not exposed by the WASM loader."));
      }
    });
    script.addEventListener("error", () => {
      reject(new Error(`Failed to load ${loaderUrl}.`));
    });
    document.head.appendChild(script);
  });
}

export function useWasm() {
  return useContext(WasmContext);
}

export function WasmProvider({ children }) {
  const [Module, setModule] = useState(null);
  const [isReady, setIsReady] = useState(false);
  const [error, setError] = useState(null);

  useEffect(() => {
    let isMounted = true;
    let bridgeManager = null;

    async function initializeRuntime() {
      try {
        const loaderUrl =
          import.meta.env.VITE_NEURAL_ENGINE_LOADER ?? DEFAULT_ENGINE_LOADER;
        const createNeuralEngine = await loadCreateNeuralEngine(loaderUrl);
        const canvas = document.getElementById("canvas");

        let resolveRuntimeInitialized;
        const runtimeInitialized = new Promise((resolve) => {
          resolveRuntimeInitialized = resolve;
        });

        const moduleConfig = {
          canvas,
          locateFile: (path) => {
            if (path.endsWith(".wasm")) {
              return `/wasm/${path}`;
            }
            return path;
          },
          onRuntimeInitialized() {
            resolveRuntimeInitialized();
          },
        };

        const initializedModule = await createNeuralEngine(moduleConfig);
        await runtimeInitialized;

        if (!isMounted) {
          return;
        }

        bridgeManager = createBridgeManager(initializedModule);
        attachPrimitiveBridge(initializedModule, bridgeManager);

        setModule(initializedModule);
        setIsReady(true);
      } catch (caughtError) {
        if (isMounted) {
          setError(caughtError);
        }
      }
    }

    initializeRuntime();

    return () => {
      isMounted = false;
      if (bridgeManager && typeof bridgeManager.delete === "function") {
        bridgeManager.delete();
      }
    };
  }, []);

  const value = useMemo(
    () => ({
      Module,
      isReady,
      error,
    }),
    [Module, isReady, error],
  );

  if (!isReady) {
    return (
      <WasmContext.Provider value={value}>
        <div className="loading-screen" role="status" aria-live="polite">
          <div className="loading-spinner" aria-hidden="true" />
          <p className="loading-copy">
            {error ? "Neural engine unavailable." : "Initializing Neural Dynamics..."}
          </p>
        </div>
      </WasmContext.Provider>
    );
  }

  return <WasmContext.Provider value={value}>{children}</WasmContext.Provider>;
}
