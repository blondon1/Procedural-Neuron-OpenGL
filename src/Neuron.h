#pragma once
#include "CellularMorphology.h"

// The Manager Class which owns the biology and the state
class Neuron {
private:
    glm::vec2 position; 

    // LIF Model State Variables (For later use)
    float membrane_potential = -65.0f;
    float resting_potential = -65.0f;
    float threshold = -50.0f;

    // Composition: The Neuron HAS these structural parts
    Soma soma;
    Cytoplasm cytoplasm;
    Nucleus nucleus;
    Dendrite dendrite;
    Axon axon;
public:
    // Called once from main() after window creation
    void initializeHardware();

    // Called every frame inside the render loop
    void Draw(unsigned int shaderProgram);
    
};