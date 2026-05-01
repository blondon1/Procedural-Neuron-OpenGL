# In Silico Neural Dynamics Simulator

A C++20 Leaky Integrate-and-Fire neural simulator compiled to WebAssembly, running natively in the browser with a React + Tailwind UI.

## Live Demo

[INSERT VERCEL URL]

## Tech Stack

- C++20
- Emscripten/WASM
- WebGL2
- React
- Vite
- Tailwind
- Web Speech API

## AI Collaboration

This project was built with a split between hand-engineered simulation work and AI-assisted integration work.

Human-engineered:

- The Leaky Integrate-and-Fire physics model, including membrane potential updates, refractory timing, stimulus injection, and synaptic propagation behavior.
- The memory architecture using `std::vector<std::unique_ptr<Neuron>>` to keep neuron addresses stable while synapses store target pointers.
- The procedural topology system, including spatial placement and rejection sampling to reduce cell overlap.
- The Signed Distance Field shader math used for membrane blending and cellular rendering.

AI-assisted:

- Emscripten and CMake build configuration for compiling the native C++ engine to WebAssembly.
- The `embind` data bridge exposing primitive, index-based getters from C++ to JavaScript.
- React/Vite UI scaffolding, including the WASM loading provider and floating telemetry panels.
- The Web Speech API narration controller used for the guided tour.

The core simulation behavior was written and validated by hand. AI was prompted for implementation assistance around portability, frontend architecture, build configuration, and browser integration, then the generated code was reviewed and adapted in the repository.

## How to Run Locally

```bash
git clone <repo-url>
cd Neurons-main/web-ui
npm install
npm run dev
```

Open the local Vite URL printed by the terminal.

## WebAssembly Binary

The `.wasm` binary is committed pre-compiled for deployment. Rebuilding the WebAssembly artifacts locally requires the Emscripten SDK (`emcmake`, `emmake`, and `emcc`) installed on your machine.
