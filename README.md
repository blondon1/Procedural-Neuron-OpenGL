# In Silico Neural Dynamics Simulator

A biologically grounded, GPU-accelerated 2D simulation of a massive, multi-polar neural graph utilizing low-level C++20 and the OpenGL graphics pipeline.

| Binary Synaptic Transmission | Tissue-Scale Topology |
| :---: | :---: |
| <video src="https://github.com/user-attachments/assets/d2ba8187-76ed-4657-a758-0d191cb91fd9" width="100%" autoplay loop muted playsinline></video> | <img src="./artifacts/macro_network.png" width="100%"> |
| *LIF Physics & Kinematic Signal Propagation* | *1,000-Node Algorithmic Graph & Synaptic Web* |

## Project Overview
This project provides a high-performance, visually intuitive prototyping environment for computational neuroscience. Existing tools (like NEURON or Brian2) have steep learning curves and lack real-time visual feedback for spatial topologies. This engine bridges that gap. By bypassing high-level wrappers, it utilizes custom C++ memory allocation and OpenGL shaders to simulate the continuous spatial propagation and Leaky Integrate-and-Fire (LIF) physics of a massive, procedurally generated neural tissue graph. 

It is designed to serve as an interactive teaching and prototyping sandbox for observing emergent network-level dynamics.

## Engineering Journal
A transparent, chronological log of architectural decisions, research findings, and technical friction is maintained in the `artifacts/` directory. This journal serves as a strict mathematical and structural record of the project's evolution.
**Read the full log here:** [Engineering_Journal.md](./artifacts/Engineering_Journal.md)

## Key Engineering Milestones
* **Autonomous LIF Physics Engine:** Engineered a Leaky Integrate-and-Fire model decoupled from the render loop via a fixed-step accumulator, ensuring mathematically deterministic membrane voltage decay, action potential thresholds (-55mV), and strict temporal refractory lockouts regardless of GPU frame rates.
* **Heap-Allocated Memory Architecture:** Refactored the core execution loop into a `NeuralNetworkManager` utilizing `std::vector<std::unique_ptr<Neuron>>` to guarantee immutable memory addresses, strictly preventing `Synapse` pointer invalidation during massive dynamic array resizing.
* **Algorithmic Graph Topology:** Replaced rigid grid placements with an $O(N^2)$ Euclidean proximity loop, dynamically fusing weighted synapses between any nodes falling within a 4.0-unit biological radius, overlaying a massive `GL_LINES` synaptic web.
* **Spatial Validation (Rejection Sampling):** To ensure organic cellular territoriality, the procedural scatter algorithm utilizes Rejection Sampling to enforce a 1.5-unit biological minimum radius between somas, entirely eliminating geometric clipping.
* **Organic Membrane Webbing (SDF Blending):** Programmed custom Fragment Shaders utilizing Signed Distance Fields (SDF) and the `smoothstep` function to natively calculate continuous mathematical membrane blending between dendrites and the cellular core.

## Interactive Controls
The simulation features a mathematically scaled, dynamic orthographic camera:
* **W, A, S, D:** Planar translation (Pan Up, Left, Down, Right).
* **UP / DOWN Arrows:** Zoom In / Zoom Out (Translation speed scales dynamically with focal length).

## Current Research & Development Roadmap
This project is an active **Work In Progress (WIP)**, transitioning from a static topology to a dynamic Spiking Neural Network (SNN).
- [x] **Phase 5**: 1,000-Node Procedural Generation and Euclidean topological networking.
- [ ] **Phase 6**: Network Stabilization via Inhibitory (GABAergic) neurons to mathematically balance excitation and prevent hyper-synchronous seizure loops.
- [ ] **Phase 7**: Parameter calibration against the Brunel (2000) sparse network dynamics model to achieve asynchronous irregular (AI) firing states.
- [ ] **Phase 8**: Implementation of Spike-Timing-Dependent Plasticity (STDP) to simulate Hebbian network learning and structural rewiring.

## Build Instructions

### Prerequisites
* C++20 Compatible Compiler (MSVC, GCC, or Clang)
* [CMake](https://cmake.org/)
* [vcpkg](https://vcpkg.io/en/)

### Steps
1. **Clone the repository**:
   ```bash
    git clone [https://github.com/blondon1/Procedural-Neuron-OpenGL](https://github.com/blondon1/Procedural-Neuron-OpenGL)
    cd Procedural-Neuron-OpenGL
2. **Install Dependencies (vcpkg)**:
   Ensure `vcpkg` is installed and integrated. This project requires the following libraries:
   ```bash
    vcpkg install glfw3 glad glm
3. **Configure and Build**:
   Create a build directory and run CMake, pointing to your vcpkg toolchain file.
   ```bash
    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake
    cmake --build .
4. **Run the Simulation:**:
   Execute the compiled binary from the build directory.
   ```bash
    .\build\Debug\Neurons2D.exe

## License
This project is licensed under the **MIT License** - see the [LICENSE](https://opensource.org/license/MIT) file for details.
