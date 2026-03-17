#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

class NeuralNetworkManager {
private:
    // Heap-allocated memory anchor to strictly prevent Synapse pointer invalidation
    std::vector<std::unique_ptr<Neuron>> neurons;

public:
    NeuralNetworkManager() = default;
    ~NeuralNetworkManager() = default;

    // Reconstructs the exact binary synapse we currently have
    void InitializeBinarySynapse();

    // Centralized execution controllers
    void Update(float deltaTime);
    void Draw(unsigned int shaderProgram);
};