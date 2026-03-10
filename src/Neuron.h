#pragma once
#include "CellularMorphology.h"

// The Manager Class which owns the biology and the state
class Neuron {
private:
    glm::vec2 position; 
    // Composition: The Neuron HAS these structural parts
    Soma soma;
    Cytoplasm cytoplasm;
    Nucleus nucleus;
    Dendrite dendrite;
    Axon axon;

    // LIF BIOLOGICAL CONSTANTS(mV)
    float restingPotential = -70.0f;   // The biological baseline
    float thresholdPotential = -55.0f; // The voltage required to trigger an Action Potential
    float resetPotential = -80.0f;     // The hyperpolarization dip after firing
    float timeConstant = 15.0f;        // The resistance/capacitance decay coefficient

    // Live state
    float membranePotential = -70.0f;  // The live fluctuating voltage 

public:
    // Called once from main() after window creation
    void initializeHardware();

    // Called every frame inside the render loop
    void Draw(unsigned int shaderProgram);

    void Update(float deltaTime);

    void InjectStimulus(float millivolts);
    
};