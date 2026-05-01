# In Silico Neural Dynamics Simulator

In Silico Neural Dynamics Simulator is a biologically grounded C++20 neural simulation engine ported to the web through WebAssembly for real-time interactive prototyping. The project models a procedurally generated 1,000-node neural graph using Leaky Integrate-and-Fire (LIF) dynamics, fixed-timestep simulation, GPU-rendered cellular morphology, and a React interface designed for fast inspection during a live demo.

The goal is to make computational neuroscience feel immediate: judges can observe membrane voltage, spatial topology, action-potential behavior, and guided narration directly in the browser without installing a native graphics stack.

## Building with AI

The core LIF physics and custom memory allocation were strictly human-engineered. The membrane voltage model, refractory timing behavior, synaptic signal flow, and heap-anchored `std::unique_ptr<Neuron>` architecture were designed manually to keep the biological simulation deterministic and memory-safe.

AI was used as an engineering accelerator around the human-authored core:

- Calculated the complex Signed Distance Field (SDF) math used by the shaders to blend dendritic membranes smoothly into cellular bodies.
- Architected the Emscripten/WebAssembly compilation pipeline and the JavaScript data bridge used by the React frontend.
- Generated the Vite/React UI, high-frequency `useRef` telemetry panel, and Web Speech API narration engine while navigating strict Apple HIG dark-mode design constraints.

## Tech Stack

- C++20 simulation engine
- WebGL2 and Emscripten/WebAssembly
- React with Vite
- Tailwind CSS
- Web Speech API

## What to Look For

- A procedurally generated 1,000-node neural graph rendered through WebGL-compatible buffers.
- Fixed-timestep LIF physics designed to remain stable independent of display refresh rate.
- A high-frequency selected-neuron panel that samples membrane voltage at animation-frame speed without triggering React state churn.
- A cinematic guided tour that narrates the simulation through the browser's speech synthesis system.
- Apple HIG-inspired dark UI: pure black canvas, thin translucent borders, subtle glass panels, and system typography.

## How to Run

Live demo: [Insert Vercel Link Here]

1. Open the Vercel link in a modern desktop browser.
2. Wait for the WebAssembly runtime gate to finish initializing the neural engine.
3. Use the floating selected-neuron panel to observe live membrane voltage.
4. Click **Start Guided Tour** to launch the narrated walkthrough.
5. Interact with the canvas directly; the UI overlay is configured so non-panel clicks pass through to the simulation controls.

## Local Development

From the React frontend directory:

```bash
cd web-ui
npm install
npm run dev
```

For production deployment, the configured build script runs the Emscripten build before the Vite frontend build:

```bash
cd web-ui
npm run build
```

The full production build expects the Emscripten toolchain (`emcmake` and `emmake`) to be available on `PATH`.

## License

This project is licensed under the MIT License. See [LICENSE](./LICENSE) for details.
