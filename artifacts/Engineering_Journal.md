**The purpose of this journal is to provide a transparent, chronological log of my engineering decisions and problem-solving process for anyone reviewing this repository.**

# 2/26/2026
I built the foundations for the project using CMAKE as the build system with the vcpkg as the package manager, declared the gitignore filters for project organization, I'm using c++ 20 as the standard of my project(currently reading a tour of c++ by Bjarne Stroustrup so I'm applying the new guidelines), first time touching graphics programming on c++ so this is going to be fun

I decided to have my human readable code inside the src folder, and will begin preparing the systems architecture and requirement documents to define the scope, biological constraints,objectives for this development lifecycle.

It's amazing how they use fractal geometry to have better connectivity like other anatomical 
systems in the human body
Link: *https://www.nature.com/articles/s41598-021-81421-2*

Neural models are quite varied in complexity and in how many factors they consider, I think I should use the Leaky Integrate and Fire(LIF) model because of its simplicity, it is still a highly efficient linear encoder.
Lecture from Dr. Mario Negrello from the Erasmus Medical Center, Netherlands
Link: *//www.youtube.com/watch?v=xx45K9fuuO4&t=47s*

# 2/27/2026
I need to understand my environment deeply before I start engineering solutions for the problems of showcasing two neurons talking to each other through electrical signals first in 2D and then in 3D.

## Iterative process
**Source Code(as C++ 20)** > **CMake(as the generator)** > **MSVC(as the compiler)** > **Executable(.exe)**

## Tech Stack:
**C++ 20** modern standard
**CMake** build generator with instructions to the compiler
**vcpkg** package manager with dependencies
**OpenGL** is the API that dictates the guidelines to talk to the GPU
**GLFW** provides OpenGL with the container(window) for its context(textures, colors, shaders) and links them together
**GLM** handles the math to generate the matrices/vectors(math library) and the fundamental geometry of the simulation at the lowest level will be constructed using GPU optimized triangles
**glad** loads the modern functions of OpenGL 4.6 to the memory using pointers since Windows has those functions hidden and only knows how to talk to OpenGL 1.1 from 1997 without it.

## Biological Anatomy
The soma(body of the neuron) has the ability to send electrical signals through the axon and receive electrical signals from the dendrites to communicate with other neurons, I need to simulate first the body of the cell by iterating over the area of the circle we can get the sine and cosine for each angle and multiply them by the radius to get a circle.

![e.g.](Complete_neuron_cell_diagram.png)

# 2/28/2026
OpenGL has been a learning experience in udnerstanding the pipeline between CPU logic and raw GPU execution. At first, I was treating the graphics pipeline like a standard script—trying to force GLSL directly into my C++ environment and misunderstanding how the state machine tracks memory. I had to tear that down and rebuild my mental model around the strict Core Profile architecture: using VBOs to essentially rent out raw VRAM for my coordinate payloads, and VAOs as the orchestrators that tell the graphics card exactly how to parse that memory during the render loop. The real turning point was moving past hardcoded, static float arrays. Figuring out why my four vertices initially rendered a right triangle instead of a full rectangle forced me to understand how OpenGL fundamentally rasterizes primitives. Now, instead of manually plotting points, I'm shifting to procedural generation—encapsulating the math, into object-oriented structures that dynamically calculate and push coordinates into vector buffers. 

## Brainstorming on engineering dilemmas for the graphics programming simulation

Need a circle and an oval so the GL_TRIANGLE_FAN primitive should work for the soma and nuclei, using a central point and triangles from it and using the sin and cosine to calculate the radius, mark points on the circumference and connect them, when there's a big enough number of them the regular polygon will have so many straight edges that it looks like a smooth circle to the human eye

L-Systems algorithms are used to simulate the growth of fractal topologies like plants so I think I could apply this to the dendrites.  

![e.g.](LSystem.png)

I should use an an **Orthographic Projection Matrix construct** to allow definitions of C++
coordinates using biological units like micrometers, μm

![e.g.](ortho-proj3.png)

**Subdivided Geometry** vs **Shader UV Propagation** for the LIF modeling of exponential charge in the neurons, **Subdivided Geometry** may cause the dentrite to have too many segments and LIF equations so it will scale terribly so **Shader UV Propagation** should allow me to unify the entire dentrite mathematically(LIF equation per neuron)

**Notes:**
The soma is essentialy a biological capacitor filled with ions
Key concepts of the LIF model: the neurons sums up electrical signals from other neurons, without input the voltage(electrical charge) slowly leaks away but if it crosses a threshold it creates a spike and after that it resets the voltage.

![e.g.](Leaky_Integrate-and-Fire_model_neuron.jpg)

![e.g.](neuron_anatomy.jpg)

# 3/1/2026

OOD(Object Oriented Design)
So I have the following components that I have to implement in code, the nucleus as the DNA holder of the cell, the soma, the dendrites and the axon, firstly I want to focus on the nucleus and the soma to have a visual representation with a dark blue for the nucleus and a light blue for the soma, I will have a struct for all of them and a Neuron class to own the building parts of the cell, because there is a difference between has-a and is-a in OOD and a neuron owns its parts, but its parts do not inherit from them. Need a base class for the architecture to contain the VBO and VAO context to not have to repeat myself in the main function. The struct building components of the cell will inherit from the base class renderShape(not the Neuron class). The base class will 1- declare the VBO and VAO variables 2- generate the math on the cpu side for the shapes 3- generate the VBO object and VAO state manager on the GPU 4- draw the elements.


# 3/2/2026
Quote from The Algorithmic beauty of plants
**"Self-similarity relates plant structures to the geometry of fractals. Computer-aided** **visualization of these structures, and the processes that create them, joins**
**science with art."** This is an amazing book that explains the application of computer graphics to biological structures, and it is a beautiful intersection of biology and computer science/mathematics.

Link: https://algorithmicbotany.org/papers/abop/abop.pdf

**Debugging dendrites:**
A simnple L-system is too deterministic, and the dendrite branches need to originate from the cell body(soma)'s perimeter, rather than cartesian coordinates.
I need to anchor the geometry to have a natural cone like structure at the base because dendrites need to look like extentions of the cell body and the branching process needs more noise and randomness to look truly organic and natural. 

I need to calculate starting coordinates for **each primary dendrite branch**, to be able to simulate a multipolar neuron, and make sure each new branch grothws outward in coorelation to the neuron's center. 

**The parametric equation of a circle** which simplifies complex coordinate geometry to plot points along a circle's perimeter should help me place the locations.

Assuming the soma's center and its radius, any unknown point in its perimeter can be define by the angle θ, then divide the 360 circle(2pi radians) into equal slices, the baaseline angle θ for each i numPrimaryDendrite is calculated using θ_i ​= 2π ⋅ i/N​ 

I need randomization of the placement of the primaries dendrites and randomization of the dendrites to curve dynamically:

To remove straight-line branches, the current L-system forward command (F) of length L is fractured into sub-segments (k)
subSegments using linear subdivision: l = L/K

vector projecttion knowing the Turtle's heading, to calculate its next position:
iterate j from 1 to k and at each step, there's a generated angular drift(Delta ⋅ Alpha) 
x_j ​= x_j − 1 ​+ lcos(α_j−1​)
y_j ​= y_j − 1 ​+ lsin(α_j−1​)

Note: the perpendicular vector must be recalculated at every micro-step j

α_j ​= α_j − 1 ​+ Delta ⋅ Alpha

**Dendrites organic shapes**
Need to solve the geometric friction on the CPU by mutating the underlying data structure, or on the GPU by manipulating the visual output of curve the base vertices of the primary dendrites.

**Challenges:**
1. the C++ geometry is produceding harsh corners where the dendrites met the soma.
2. the L-system is producing identical "snowflake" branches across every primary dendrite.
3. primary dendrites rendered as rectangular tubes, they should look like a natural extension of the cell body.
4. the flat base of the initial trapezoids were creating empty triangular gaps in the visual image.

**Solutions:**
1. intercep the FragPos from the vertex shader and injected a signed distance Field (SDF) into the fragment shader.
2. replace the rigid F[- F][+ F] mutation loop.
3. replaced the static rectangular shapes in the linear subdivision loop with decaying trapezoids to make the primary dendrites look like extentions of the soma
4. submerged the origin coordinate of the L-system into the core of the cell by testing it manually until it looked natural

3/4/2026
Now that I have a working demo of the neuron morphology, I will encapsulate my classes using Object-Oriented Design (OOD) principles. I am organizing my sections of logic into different files to ensure faster compile times as the project scales and to enforce strict structural organization overall. Blueprint declarations will be isolated into .h header files, while massive geometric functions will be extracted into .cpp implementation files accordingly.

# 3/8/2026
![e.g.](neuron_anatomy.jpg)
Now that the OOD refactoring is complete and the translation units are compiling cleanly, today's goal is to implement the complete axon morphology from start to finish. I am organizing the logic into strict phases so I can verify the geometry visually at each step and avoid massive debugging headaches. Phase 1 reserves a spatial exclusion zone on the right side of the soma by tweaking the dendrite arc math, giving the axon hillock a dedicated slot without overlapping. Phase 2 will weld a long, uniform transmission shaft directly to the hillock's coordinates to keep it as a single, optimized OpenGL draw call. For Phase 3, I plan to attach a shallow L-System at the end of the shaft for the axon terminals, using fewer iterations and thicker endpoints for the synapses. I am purposefully deferring the myelin sheaths for now to keep the complexity low and ensure the foundational MVP is mathematically sound before adding extra mesh layers.

# 3/14/2026
**Objective:** Upgrade from static geometry to dynamic computational physics and multi-agent networking.

**The Physics Engine (LIF Model)**

1. Chronometer: Bound the engine to real-world time via deltaTime to ensure frame-independent decay.

2. Euler Integration: Simulated the biological membrane leak using discrete linear algebra steps (y=mx+b) instead of heavy continuous calculus.

3. Action Potential & Lockout: Enforced a hard −55.0 mV firing threshold. Engineered a 50.0 ms absolute refractory period (a temporal lock) to force the membrane to ignore stimulus post-firing, mimicking closed sodium channels.

**The Graph Topology (Networking)**

1. Hardware Translation (O(1) Memory): Upgraded the Vertex Shader with a model matrix. Neurons are drawn once at the origin and translated directly on the GPU, allowing massive scaling without memory bloat.

2. Topological Pointers: Built a Synapse struct (a directed edge) holding the target neuron's memory address and a neurotransmitter voltage weight.

3. Communication: Upon firing, the presynaptic neuron forcefully executes an InjectStimulus command on its connected targets.

**Result:** Overpowering the presynaptic leak triggers a mathematically verifiable chain reaction across the graph. Due to the refractory chronometers, the network displays emergent biological desensitization: if the target is spammed too quickly, it physically rejects the incoming signal.


I transitioned the engine from a hardcoded binary synapse to a procedurally generated neural graph, but scaling up immediately exposed a fatal C++ memory hazard. I realized that if I just dumped dozens of neurons into a standard std::vector, dynamic array resizing would shift the physical memory blocks, instantly turning every Synapse target into a dangling pointer and guaranteeing a massive segmentation fault. Applying the Steel-Man rule to my architectural options, rigid stack arrays were a dead end for a dynamic system, so I refactored the core loop into a centralized NeuralNetworkManager using strict heap allocation (std::vector<std::unique_ptr<Neuron>>) to permanently lock down those memory addresses. With the pointers mathematically stabilized, I tackled the spatial topology. A rigid grid is computationally cheap but biologically false, so I scattered the nodes using procedural generation; however, pure randomness caused severe geometric clipping and overlapping somas. To fix this, I implemented a Rejection Sampling algorithm to enforce a strict 1.5-unit biological territory, automatically discarding and rerolling coordinates until the tissue visually verified as an organic mesh. Finally, to wire it all together without writing manual pointers, I built an O(N^2) Euclidean initialization loop that applies the Pythagorean theorem to dynamically fuse weighted synapses between any nodes falling within a 4.0-unit radius. The result is exactly the self-organizing system I was aiming for: a stable, non-clipping neural tissue where universal stochastic noise triggers localized action potentials that autonomously cascade across the algorithmically wired graph.

# 3/18/2026

Objective: Topological Polish, Viewport Unconstraint, and Stress-Testing Scale

After securing the procedural topology, I hit an immediate aesthetic and diagnostic wall. The mathematically perfect graph looked entirely synthetic because every neuron strictly aligned to 0 radians, pointing dead right. I initially considered mathematically bending the procedural L-System axons to physically touch their target dendrites. However, since a single neuron connects to multiple neighbors in opposite directions, calculating dynamic fractal branching for multi-target pathfinding is an O(N3) computational nightmare.

Instead, I opted for an  abstraction. I injected a stochastic rotational matrix (glm::rotate) during instantiation, assigning a random 0 to 2π radian angle to give the cells a chaotic, microscopic organic scatter. To solve the unreadable "snowstorm" of traveling packets, I safely exposed the encapsulated synaptic pointers via a constant reference getter and batched a single GL_LINES dispatch to render a faint, 15% opacity cyan topological web in the background. The structural integrity of the graph was finally visible.

With the network now readable, the static camera became a critical bottleneck; I was completely restricted from exploring the tissue. I engineered a dynamic WASD planar translation system but immediately encountered friction with the movement scaling: panning a static float distance feels impossibly sluggish when macro-zoomed and violently uncontrollable when micro-zoomed. The strict mathematical fix was binding the translation speed directly to the cameraZoom state variable, ensuring 1:1 optical consistency regardless of the focal length.

Finally, I stress-tested the engine's scaling limits. I forced the architecture to reveal its three hard bottlenecks: the spatial geometry trap (packing circles into a 12×12 grid causes infinite rejection-sampling loops), the CPU O(N2) wiring loop, and the OpenGL draw-call ceiling. To shatter the immediate spatial limit, I drastically expanded the procedural canvas to a 200×200 bounding box and pulled the macro-camera back to a 100.0 zoom factor.

The engine successfully brute-forced a massive, sweeping 1,000-node network. Witnessing thousands of discrete action potentials cascading across the glowing synaptic web verified the robustness of the architecture, while simultaneously proving my next mandatory engineering threshold: scaling beyond this to 10,000+ nodes will strictly require abandoning individual glDrawArrays calls and implementing Instanced Rendering to bypass the CPU-GPU data bus bottleneck.

# 3/23/2026

The engine successfully pushed a 1,000-node topology today. It is mathematically stable and visually striking, but watching the cascades propagate, I had to stop and analize my own underlying premise.

If a critic looked at this right now, their strongest argument would be this: You haven't built a computational engine; you've built a highly optimized biological screensaver. And they would be mathematically right.

My immediate instinct was to chase scale. I was mapping out Phase 6 to rewrite the pipeline for glDrawArraysInstanced to shatter the GPU bottleneck and force 10,000+ nodes onto the screen. But why? What does 10,000 nodes actually solve that 1,000 doesn't, if the network can't process a single byte of persistent data? Chasing draw-calls is an engineering vanity metric. I am optimizing for the wrong vector. A 2D planar constraint, LIF physics, and O(N2) Euclidean wiring.

This shouldn't be a simulation of a brain. This is an in silico digital twin of a Microelectrode Array (MEA) petri dish.

When researchers at places like Cortical Labs plate living cells onto silicon grids to teach them to play Pong (Organoid Intelligence), they are working within the exact same 2D spatial constraints my engine currently enforces. If I lean into this and treat the canvas as a virtual petri dish, the software suddenly shifts from a graphics project into a theoretical biocomputing sandbox. Could I use this to prototype electrode placements? Could I isolate the exact spatial degradation of synaptic pruning before a researcher wastes living in vitro tissue?

But leaning into this exposes the engine's current flaw.

The network is 100% Excitatory (Glutamatergic). It only knows how to add voltage. If I inject sustained noise into this dense Euclidean web, it is mathematically guaranteed to trigger an infinite feedback loop. It's not processing information; it is simulating a seizure.

To make this petri dish functional, it requires biological brakes. The next architectural mandate isn't scaling up; it's stabilizing what is already here.

**Open Questions for Phase 6:**

1. If I inject Inhibitory (GABAergic) neurons into the procedural generation pool at a strict 20% biological ratio, what happens to the emergent topology?

2. Will the negative voltage (-mV) perfectly counterbalance the cascades and create stable, rhythmic oscillations? Or will it over-correct and mathematically silence the entire dish?

3. How do I visually differentiate a Glutamatergic node from a GABAergic node in the shader without breaking the organic aesthetic?

4. If I can achieve a stable, rhythmic oscillation, what is the mathematical threshold to introduce Spike-Timing-Dependent Plasticity (STDP)? Can I actually get this digital tissue to "learn" a pathway?