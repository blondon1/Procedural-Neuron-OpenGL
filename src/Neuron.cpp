#include "Neuron.h"
#include <iostream>

// Called once from main() after window creation
void Neuron::initializeHardware() {
    soma.allocateVram();
    cytoplasm.allocateVram();
    nucleus.allocateVram();
    dendrite.generateFractalTopology(4, 0.3f, 7, 0.08f, 22.0f, 0.35f);
    dendrite.allocateVram();
    axon.generateAxonTopology(0.3f);
    axon.allocateVram();
    
}

// Called every frame inside the render loop
void Neuron::Draw(unsigned int shaderProgram) {
    // Normalize the voltage to a 0.0 to 1.0 scale
    // -70mV = 0.0 (Resting) | -55mV = 1.0 (Action Potential Threshold)
    float normalizedActivity = (membranePotential - restingPotential) / (thresholdPotential - restingPotential);
    
    // Clamp the value to strictly prevent color inversion bugs if voltage overshoots
    if (normalizedActivity < 0.0f) normalizedActivity = 0.0f;
    if (normalizedActivity > 1.0f) normalizedActivity = 1.0f;

    // Interpolate the color channels
    // Base Color: R: 0.0, G: 0.651, B: 1.0
    // Peak Color: R: 1.0, G: 1.0,   B: 1.0
    float currentR = 0.0f + (1.0f * normalizedActivity);
    float currentG = 0.651f + (0.349f * normalizedActivity);
    float currentB = 1.0f; // The blue channel remains fully saturated


    
    dendrite.Draw(GL_TRIANGLES, shaderProgram, currentR, currentG, currentB, 1.0f, glm::vec2(0.0f, 0.0f), 0.0f);
    axon.Draw(GL_TRIANGLES, shaderProgram, currentR, currentG, currentB, 1.0f, glm::vec2(0.0f, 0.0f), 0.0f);
    soma.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f);
    cytoplasm.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.15f, 0.75f, 1.0f, 1.0f);
    nucleus.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.0f, 0.38f, 1.0f);
    
}

void Neuron::InjectStimulus(float millivolts) {
    // Check the chronological lock. If active, silently reject the hardware interrupt
    if (currentRefractoryTime > 0.0f) {
        return; 
    }
    
    // If the channels are open, accept the current
    membranePotential += millivolts;
    std::cout << "[HARDWARE] Stimulus Injected. Current Vm: " << membranePotential << " mV\n";
}

void Neuron::Update(float deltaTime) {
    float dt_ms = deltaTime * 1000.0f;

    // --- STATE 1: THE REFRACTORY LOCKOUT ---
    if (currentRefractoryTime > 0.0f) {
        // Count down the biological clock
        currentRefractoryTime -= dt_ms;
        
        // Rigorously clamp the voltage to the reset state while channels recover
        membranePotential = resetPotential; 
        
        // Halt all further physics calculations this frame
        return; 
    }

    // --- STATE 2: THE LIF INTEGRATION ---
    if (membranePotential >= thresholdPotential) {
        // The neuron fires. Trigger the Action Potential and initiate the chronological lock.
        membranePotential = resetPotential; 
        currentRefractoryTime = absoluteRefractoryPeriod; 
        
        std::cout << "[PHYSICS] ACTION POTENTIAL FIRED! Entering Refractory Lockout.\n";
    } else {
        // Standard exponential decay
        float derivative = (restingPotential - membranePotential) / timeConstant;
        membranePotential += derivative * dt_ms;
        
        if (membranePotential > restingPotential + 0.1f) {
            std::cout << "[PHYSICS] Leaking... Vm: " << membranePotential << " mV\n";
        }
    }
}