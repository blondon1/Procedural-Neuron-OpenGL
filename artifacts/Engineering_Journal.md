**The purpose of this journal is to provide a transparent, chronological log of my engineering decisions and problem-solving process for anyone reviewing this repository.**

# 2/26/2026
I built the foundations for the project using CMAKE as the build system with the vcpkg as the package manager, declared the gitignore filters for project organization, I'm using c++ 20 as the standard of my project(currently reading a tour of c++ by Bjarne Stroustrup so I'm applying the new guidelines), first time touching graphics programming on c++ so this is going to be fun

I decided to have my human readable code inside the src folder, and will begin preparing the systems architecture and requirement documents to define the scope, biological constraints,objectives for this development lifecycle.

It's amazing how they use fractal geometry to have better connectivity like other anatomical 
systems in the human body
Link: *https://www.nature.com/articles/s41598-021-81421-2*

Neural models are quite varied in complexity and in how many factors they consider, I think I should use the Leaky Integrate and Fire(LIF) model because of its simplicity, it is still a highly efficient linear encoder.
Lecture from Dr. Mario Negrello from the Erasmus Medical Center, Netherlands
https: *//www.youtube.com/watch?v=xx45K9fuuO4&t=47s*

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
