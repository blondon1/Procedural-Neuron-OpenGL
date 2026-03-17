#include "NeuralNetworkManager.h"
#include <cstdlib> // Required for rand()

void NeuralNetworkManager::InitializeProceduralGraph(int numNeurons) {
    for (int i = 0; i < numNeurons; i++) {
        // Instantiate on the heap
        auto newNeuron = std::make_unique<Neuron>();
        newNeuron->initializeHardware();

        // Generate stochastic spatial coordinates (Spread from -6.0f to +6.0f)
        float randomX = ((static_cast<float>(rand()) / RAND_MAX) * 12.0f) - 6.0f;
        float randomY = ((static_cast<float>(rand()) / RAND_MAX) * 12.0f) - 6.0f;

        // Apply the matrix offset
        newNeuron->SetPosition(randomX, randomY);

        // Anchor to memory
        neurons.push_back(std::move(newNeuron));
    }

    // Define the critical biological radius. Nodes closer than 4.0 units will connect.
    float connectionRadius = 4.0f; 

    for (size_t i = 0; i < neurons.size(); i++) {
        for (size_t j = 0; j < neurons.size(); j++) {
            // A biological neuron does not form a chemical synapse with its own soma
            if (i == j) continue;

            glm::vec2 posA = neurons[i]->GetPosition();
            glm::vec2 posB = neurons[j]->GetPosition();

            // GLM handles the heavy Pythagorean square root math natively
            float distance = glm::distance(posA, posB);

            if (distance <= connectionRadius) {
                // Procedurally generate a variable neurotransmitter weight (10.0mV to 18.0mV)
                // This ensures the network topology isn't perfectly uniform
                float randomWeight = 10.0f + (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 8.0f)); 
                
                neurons[i]->AddSynapse(neurons[j].get(), randomWeight);
            }
        }
    }
}

void NeuralNetworkManager::Update(float deltaTime) {
    // Universal Stochastic Background Noise
    for (auto& neuron : neurons) {
        // 1% chance per frame for ANY neuron to receive biological noise
        if ((rand() % 100) < 1) {
            neuron->InjectStimulus(12.0f);
        }
        
        // Execute discrete physics state machine
        neuron->Update(deltaTime);
    }
}

void NeuralNetworkManager::Draw(unsigned int shaderProgram) {
    // Execute rendering dispatch for all nodes
    for (auto& neuron : neurons) {
        neuron->Draw(shaderProgram);
    }
}