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