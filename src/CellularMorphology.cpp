#include "CellularMorphology.h"

Nucleus::Nucleus() {
    float radiusX = 0.07f; // narrower width
    float radiusY = 0.10f; // taller height for an oval shape
    int visualFidelity = 36;
    generateCurvilinearShape(radiusX, radiusY, visualFidelity);
}

Soma::Soma() {
    float radius = 0.3f; // equal radius for a circle
    int visualFidelity = 36; // 36 segments for a smooth circle
    generateCurvilinearShape(radius, radius, visualFidelity);
}

Cytoplasm::Cytoplasm() {
    float radiusX = 0.25f; // narrower width
    float radiusY = 0.2f; // taller height for an oval shape
    int visualFidelity = 36; 
    generateCurvilinearShape(radiusX, radiusY, visualFidelity);
}

// Added numPrimaryDendrites and somaRadius to the parameters
void Dendrite::generateFractalTopology(int iterations, float somaRadius, int numPrimaryDendrites, float branchLength, float branchAngle, float baseThickness) {

    // PHASE 1: DNA STRING GENERATION
    std::string currentString = "F"; 

    // Initialize a dedicated random engine for the grammar phase
    std::random_device rdGrammar;
    std::mt19937 genGrammar(rdGrammar());
    std::uniform_real_distribution<float> probabilityMatrix(0.0f, 1.0f);

    for (int i = 0; i < iterations; i++) {
        std::string nextString = "";
        for (char c : currentString) {
            if (c == 'F') {
                // Roll the probability dice for this specific node
                float p = probabilityMatrix(genGrammar);
                
                if (p < 0.35f) {
                    // 35% chance: Asymmetrical Left Split
                    nextString += "F[-F]"; 
                } else if (p < 0.70f) {
                    // 35% chance: Asymmetrical Right Split
                    nextString += "F[+F]"; 
                } else {
                    // 30% chance: Symmetrical Bilateral Split
                    nextString += "F[-F][+F]"; 
                }
            } else {
                // Retain memory characters (+, -, [, ]) without mutation
                nextString += c;
            }
        }
        currentString = nextString; 
    }

    std::cout << "Stochastic Dendrite DNA (Iterations: " << iterations << "): " << currentString << std::endl;

    // PHASE 2: GEOMETRIC EXECUTION 
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Macroscopic branching (L-System nodes)
    std::uniform_real_distribution<float> macroAngleNoise(-8.0f, 8.0f);
    
    // Microscopic curvature (Segment drift)
    std::uniform_real_distribution<float> microDriftNoise(-3.0f, 3.0f);

    vertices.clear();
    float angleRad = branchAngle * (float)std::numbers::pi / 180.0f;

    // Loop to generate multipolar primary dendrites around the soma
    for (int t = 0; t < numPrimaryDendrites; t++) {
        
        // Calculate starting position on the soma's perimeter (Constrained to a 240-degree upper arc)
        float startAngle = 45.0f * (float)std::numbers::pi / 180.0f; 
        float arcSpan = 280.0f * (float)std::numbers::pi / 180.0f;

        // Cast 't' to a float to prevent integer division truncation
        float fraction = (float)t / (numPrimaryDendrites - 1);
        float theta = startAngle + (arcSpan * fraction);
        
        // Add a slight randomized offset so primary dendrites aren't perfectly symmetrical
        std::uniform_real_distribution<float> trunkOffset(-0.2f, 0.2f);
        theta += trunkOffset(gen);

        // GEOMETRIC FIX: Submerge the wide base inside the soma to close the tangent gap
        float anchorDepth = 0.045f; // How deep to bury the trapezoid base
        float submergedRadius = somaRadius - anchorDepth;

        glm::vec2 startPos;
        startPos.x = submergedRadius * cos(theta);
        startPos.y = submergedRadius * sin(theta);

        // The heading angle must be orthogonal to the circle at the starting coordinate
        TurtleState currentState = { startPos, theta, baseThickness }; 
        std::stack<TurtleState> memoryStack;

        // Execute the DNA sequence for this specific primary dendrite
        for (char c : currentString) {
            if (c == 'F') {    
                int subSegments = 4; // Subdivide the line into 4 micro-segments
                float l = branchLength / subSegments;

                for (int j = 0; j < subSegments; j++) {
                    // Calculate the end position of this micro-segment
                    glm::vec2 endPos;
                    endPos.x = currentState.position.x + cos(currentState.angle) * l;
                    endPos.y = currentState.position.y + sin(currentState.angle) * l;

                    // Calculate the perpendicular vector for thickness
                    glm::vec2 perp(-sin(currentState.angle), cos(currentState.angle));
                    
                    // Calculate Exponential Decay for this specific micro-segment
                    float startHalfThick = currentState.thickness / 2.0f;
                    
                    // Apply 15% decay per micro-step, clamped to a strict biological minimum (0.003f)
                    float nextThickness = currentState.thickness * 0.65f; 
                    if (nextThickness < 0.0040f) nextThickness = 0.0040f;
                    
                    float endHalfThick = nextThickness / 2.0f;

                    // Calculate the 4 corners of the trapezoidal quad
                    glm::vec3 bottomLeft(currentState.position.x - perp.x * startHalfThick, currentState.position.y - perp.y * startHalfThick, 0.0f);
                    glm::vec3 bottomRight(currentState.position.x + perp.x * startHalfThick, currentState.position.y + perp.y * startHalfThick, 0.0f);
                    glm::vec3 topLeft(endPos.x - perp.x * endHalfThick, endPos.y - perp.y * endHalfThick, 0.0f);
                    glm::vec3 topRight(endPos.x + perp.x * endHalfThick, endPos.y + perp.y * endHalfThick, 0.0f);

                    // Push the two triangles (6 vertices)
                    vertices.push_back(bottomLeft);
                    vertices.push_back(bottomRight);
                    vertices.push_back(topLeft);
                    
                    vertices.push_back(bottomRight);
                    vertices.push_back(topRight);
                    vertices.push_back(topLeft);

                    // Update position, save the tapered thickness, and inject stochastic curvature drift
                    currentState.position = endPos;
                    currentState.thickness = nextThickness; 
                    
                    float microDrift = microDriftNoise(gen) * (float)std::numbers::pi / 180.0f;
                    currentState.angle += microDrift;
                }
                
            } else if (c == '+') {
                float randomOffset = macroAngleNoise(gen) * (float)std::numbers::pi / 180.0f;
                currentState.angle -= (angleRad + randomOffset); 
            } else if (c == '-') {
                float randomOffset = macroAngleNoise(gen) * (float)std::numbers::pi / 180.0f;
                currentState.angle += (angleRad + randomOffset); 
            } else if (c == '[') {
                memoryStack.push(currentState);
                currentState.thickness *= 0.85f; // Adjusted taper to survive longer branches
            } else if (c == ']') {
                currentState = memoryStack.top();
                memoryStack.pop();
            }
        }
    }
}

void Axon::generateAxonTopology(float somaRadius) {
    vertices.clear();

    // Calculate the spatial anchor on the X-axis
    // Submerge the hillock base into the soma to prevent a visual tangent gap
    float anchorDepth = 0.045f;
    float startX = somaRadius - anchorDepth;
    float endX = startX + 0.2f; // Extend the stump to the right by 0.2 units

    // efine the vertical (Y-axis) taper (Height instead of Width)
    float baseHalfHeight = 0.07f;  // Massive base connecting to the eastern pole of the soma
    float tipHalfHeight = 0.025f;  // Narrow tip connecting to the future shaft

    // Construct the four geometric corners mapped to the right side
    glm::vec3 topLeft(startX, baseHalfHeight, 0.0f);
    glm::vec3 bottomLeft(startX, -baseHalfHeight, 0.0f);
    glm::vec3 topRight(endX, tipHalfHeight, 0.0f);
    glm::vec3 bottomRight(endX, -tipHalfHeight, 0.0f);

    // Push Triangle 1 Counter-clockwise
    vertices.push_back(bottomLeft);
    vertices.push_back(bottomRight);
    vertices.push_back(topLeft);

    // Push Triangle 2 Counter-clockwise
    vertices.push_back(bottomRight);
    vertices.push_back(topRight);
    vertices.push_back(topLeft);

}