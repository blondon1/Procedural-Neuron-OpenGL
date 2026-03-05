#include "Neuron.h"

// Called once from main() after window creation
void Neuron::initializeHardware() {
    soma.allocateVram();
    cytoplasm.allocateVram();
    nucleus.allocateVram();
    dendrite.generateFractalTopology(4, 0.3f, 7, 0.08f, 22.0f, 0.35f);
    dendrite.allocateVram();
    // axon.allocateVram(); 
    
}

// Called every frame inside the render loop
void Neuron::Draw(unsigned int shaderProgram) {
    dendrite.Draw(GL_TRIANGLES, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f, glm::vec2(0.0f, 0.0f), 0.0f);
    soma.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f);
    cytoplasm.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.15f, 0.75f, 1.0f, 1.0f);
    nucleus.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.0f, 0.38f, 1.0f);
    
}