#include "NeuralNetworkManager.h"
#include <cstdlib> // Required for rand()
#include <glm/gtc/type_ptr.hpp>

void NeuralNetworkManager::InitializeProceduralGraph(int numNeurons) {
    InitializeSynapseLineBuffers();

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
            candidatePos.x = ((static_cast<float>(rand()) / RAND_MAX) * 200.0f) - 100.0f;
            candidatePos.y = ((static_cast<float>(rand()) / RAND_MAX) * 200.0f) - 100.0f;
            
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

NeuralNetworkManager::~NeuralNetworkManager() {
    if (synapseVBO != 0) {
        glDeleteBuffers(1, &synapseVBO);
    }
    if (synapseVAO != 0) {
        glDeleteVertexArrays(1, &synapseVAO);
    }
}

void NeuralNetworkManager::InitializeSynapseLineBuffers() {
    if (synapseVAO != 0) {
        return;
    }

    glGenVertexArrays(1, &synapseVAO);
    glGenBuffers(1, &synapseVBO);

    glBindVertexArray(synapseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, synapseVBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    // Synaptic Web Rendering
    // We set the shader color to a very faint, semi-transparent cyan
    int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform4f(colorLoc, 0.0f, 0.4f, 0.6f, 0.15f); // 15% Opacity
    
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glm::mat4 identityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identityModel));

    int centerLoc = glGetUniformLocation(shaderProgram, "somaCenter");
    glUniform2f(centerLoc, 0.0f, 0.0f);

    int radiusLoc = glGetUniformLocation(shaderProgram, "somaRadius");
    glUniform1f(radiusLoc, 0.0f);

    glLineWidth(1.0f); // Ensure lines are thin and delicate
    std::vector<glm::vec2> synapseVertices;
    
    size_t synapseVertexCount = 0;
    for (const auto& neuron : neurons) {
        synapseVertexCount += neuron->GetSynapses().size() * 2;
    }
    synapseVertices.reserve(synapseVertexCount);

    // Iterate through every neuron and batch a line to each of its targets
    for (const auto& neuron : neurons) {
        glm::vec2 startPos = neuron->GetPosition();
        
        for (const Synapse& synapse : neuron->GetSynapses()) {
            glm::vec2 endPos = synapse.target->GetPosition();
            
            synapseVertices.push_back(startPos);
            synapseVertices.push_back(endPos);
        }
    }

    if (!synapseVertices.empty()) {
        glBindVertexArray(synapseVAO);
        glBindBuffer(GL_ARRAY_BUFFER, synapseVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(synapseVertices.size() * sizeof(glm::vec2)),
            synapseVertices.data(),
            GL_DYNAMIC_DRAW
        );
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(synapseVertices.size()));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Render neurons and packets
    for (auto& neuron : neurons) {
        neuron->Draw(shaderProgram);
    }
}
