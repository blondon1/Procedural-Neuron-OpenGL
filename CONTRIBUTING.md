# Contributing to Procedural-Neuron-OpenGL

First, thank you for your interest in contributing to this simulation. This project bridges neurobiology and low-level computer graphics, requiring a rigorous approach to both mathematical theory and hardware execution.



## Current Development Focus
I'm actively building out the biological roadmap. Contributions are highly encouraged in the following areas:
* **Phase 2:** Axon Topology (Implementing elongated, distinct procedural geometry).
* **Phase 3:** LIF Model Integration (Implementing `deltaTime` physics and voltage decay).
* **Phase 4:** Synaptic Networking (Instancing multiple neurons and managing signal propagation).



## Development Setup
Before contributing, ensure your local environment is correctly configured. 
1. Follow the **Build Instructions** in the `README.md` to set up CMake, vcpkg, and your C++20 compiler.
2. Verify that the baseline simulation compiles and runs without OpenGL context errors.



## Branching Strategy & Commits
To maintain a clean repository history, adhere to the following Git workflow:

1. **Branch Naming:** Use descriptive, categorized branch names.
   * `feature/lif-physics-engine`
   * `bugfix/sdf-blending-artifact`
   * `docs/journal-update`
2. **Commit Messages:** Write commit messages in the **imperative mood** (as a command). 
   * ✅ *Correct:* `Refactor vertex shader to accept projection matrix`
   * ❌ *Incorrect:* `Refactored the vertex shader` or `fixing shader bug`



## Architectural & Coding Standards

To ensure the simulation remains performant and memory-safe, all C++ and GLSL code must adhere to these strict constraints:

### 1. Memory Management (The GPU Boundary)
* **Never call OpenGL functions inside a C++ constructor.** Contexts may not be initialized. 
* All CPU-side math and geometric generation must occur in the constructor or designated CPU functions.
* All GPU-side allocations (`glGenVertexArrays`, `glGenBuffers`) must be isolated in the `allocateVram()` functions and called only *after* the GLFW window context is active.
* **RAII Compliance:** Any struct or class that allocates GPU memory must possess a destructor that explicitly frees it (e.g., `glDeleteVertexArrays`).

### 2. Modern C++ 20
* Utilize modern C++ paradigms. Prefer `std::numbers::pi_v<float>` over legacy `#define PI`, and utilize `std::vector` for geometric payloads rather than raw C-arrays.

### 3. Biological Accuracy
* Avoid "magic numbers." If a variable represents a biological metric (e.g., membrane resting potential of `-65.0f`), document it clearly. 
* Visual rendering should follow the Orthographic Projection scale where `1.0f` translates logically to micrometers ($\mu m$) in the biological model.



## Documentation Requirement (The Engineering Journal)
We do not treat code as a "black box." Any pull request that introduces a major architectural shift, solves a significant graphics pipeline bug, or implements a new biological concept **must** be accompanied by an update to the `artifacts/Engineering_Journal.md`. 

Outline the problem, the objective constraints, and why your specific solution was chosen.



## Pull Request (PR) Process
1. Fork the repository and create your feature branch.
2. Ensure your code complies with the Architectural Standards.
3. Test the simulation locally to ensure no VRAM leaks or silent crashes occur upon window termination.
4. Submit a PR targeting the `main` branch. 
5. In your PR description, link to any relevant Issues and provide a brief summary of the technical hurdles cleared.
