#include "NeuralNetworkManager.h"
#include <cstdlib> // Required for rand()

void NeuralNetworkManager::InitializeProceduralGraph(int numNeurons) {
    // Organic Distribution via Rejection Sampling
    float minimumCellRadius = 1.5f; // The biological territory a cell requires
    int maxAttempts = 100; // Prevent infinite recursion if the 12x12 grid fills up
    std::vector<glm::vec2> assignedPositions;

    for (int i = 0; i < numNeurons; i++) {
        glm::vec2 candidatePos;
        bool positionValid = false;
        int currentAttempt = 0;

        // Loop until a valid, empty space is found or we run out of attempts
        while (!positionValid && currentAttempt < maxAttempts) {
            // Generate a candidate coordinate inside the 12x12 bounding box
            candidatePos.x = ((static_cast<float>(rand()) / RAND_MAX) * 12.0f) - 6.0f;
            candidatePos.y = ((static_cast<float>(rand()) / RAND_MAX) * 12.0f) - 6.0f;
            
            positionValid = true; // Assume valid until proven mathematically otherwise
            
            // Check distance against all previously locked coordinates
            for (const glm::vec2& lockedPos : assignedPositions) {
                if (glm::distance(candidatePos, lockedPos) < minimumCellRadius) {
                    positionValid = false; // Territorial violation. Reject and reroll.
                    break;
                }
            }
            currentAttempt++;
        }

        // Lock in the mathematically validated coordinate
        assignedPositions.push_back(candidatePos);

        // Instantiate on the heap and map to the valid space
        auto newNeuron = std::make_unique<Neuron>();
        newNeuron->initializeHardware();
        newNeuron->SetPosition(candidatePos.x, candidatePos.y);

        // Procedurally generate a random rotation (0 to 2*PI radians)
        float randomRotation = (static_cast<float>(rand()) / RAND_MAX) * (2.0f * 3.14159265f);
        newNeuron->SetRotation(randomRotation);

        neurons.push_back(std::move(newNeuron));
    }

    // Euclidean Proximity
    float connectionRadius = 4.0f; 

    for (size_t i = 0; i < neurons.size(); i++) {
        for (size_t j = 0; j < neurons.size(); j++) {
            if (i == j) continue; // Prevent self-synapsing

            glm::vec2 posA = neurons[i]->GetPosition();
            glm::vec2 posB = neurons[j]->GetPosition();
            float distance = glm::distance(posA, posB);

            if (distance <= connectionRadius) {
                // Procedural edge weights (10.0mV to 18.0mV)
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