#pragma once
#include "RenderableShape.h"
#include <iostream>
#include <string>
#include <stack>
#include <random>

struct TurtleState {
    glm::vec2 position;
    float angle; // in radians
    float thickness;

};

// The Derived Classes: They inherit VAO, VBO, and vertices 
class Nucleus : public RenderableShape {
public:
    Nucleus();

};

class Soma : public RenderableShape {
public:
    // Constructor to define the shape
    Soma();

};

class Cytoplasm : public RenderableShape {
public:
    // Constructor to define the inner, denser shape
    Cytoplasm();

};

class Dendrite : public RenderableShape {
public:
    // Added numPrimaryDendrites and somaRadius to the parameters
    void generateFractalTopology(int iterations, float somaRadius, int numPrimaryDendrites, float branchLength, float branchAngle, float baseThickness);

};

// TODO
class Axon : public RenderableShape {};