#include "NeuralNetworkManager.h"
#include <cstdlib> // Required for rand()

void NeuralNetworkManager::InitializeBinarySynapse() {
    // Instantiate onto the heap to guarantee stable memory addresses
    auto preNeuron = std::make_unique<Neuron>();
    auto postNeuron = std::make_unique<Neuron>();

    // Initialize GPU hardware
    preNeuron->initializeHardware();
    postNeuron->initializeHardware();

    // Calibrate spatial matrix (matching your 2.5f cleft offset)
    preNeuron->SetPosition(0.0f, 0.0f);
    postNeuron->SetPosition(2.3f, 0.0f);

    // Establish topological pointer bridge (Safe because they are on the heap)
    preNeuron->AddSynapse(postNeuron.get(), 16.0f);

    // Transfer memory ownership to the vector
    neurons.push_back(std::move(preNeuron));
    neurons.push_back(std::move(postNeuron));
}

void NeuralNetworkManager::Update(float deltaTime) {
    // Stochastic Background Noise (Applied strictly to the presynaptic node)
    if (!neurons.empty() && (rand() % 100) < 5) {
        neurons[0]->InjectStimulus(12.0f);
    }

    // Execute physics state machines for all nodes
    for (auto& neuron : neurons) {
        neuron->Update(deltaTime);
    }
}

void NeuralNetworkManager::Draw(unsigned int shaderProgram) {
    // Execute rendering dispatch for all nodes
    for (auto& neuron : neurons) {
        neuron->Draw(shaderProgram);
    }
}