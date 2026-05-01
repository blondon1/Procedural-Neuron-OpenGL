#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

class NeuralNetworkManager {
private:
    // Heap-allocated memory anchor to strictly prevent Synapse pointer invalidation
    std::vector<std::unique_ptr<Neuron>> neurons;
    unsigned int synapseVAO = 0;
    unsigned int synapseVBO = 0;

    void InitializeSynapseLineBuffers();

public:
    NeuralNetworkManager() = default;
    ~NeuralNetworkManager();

    void InitializeProceduralGraph(int numNeurons);

    // Centralized execution controllers
    void Update(float deltaTime);
    void Draw(unsigned int shaderProgram);
};
