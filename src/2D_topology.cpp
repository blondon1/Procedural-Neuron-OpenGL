#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <numbers>
#include <string>
#include <stack>
#include <random>
#define _USE_MATH_DEFINES
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Define Shader Source Code (GLSL - C like)
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 projection;\n"
    "out vec2 FragPos;\n" // payload for the Fragment Shader
    "void main()\n"
    "{\n"
    "   gl_Position = projection * vec4(aPos, 1.0);\n" 
    "   FragPos = vec2(aPos.x, aPos.y);\n" // Extract spatial coordinates
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "in vec2 FragPos;\n"
    "out vec4 FragColor;\n"
    "uniform vec4 objectColor;\n" 
    "uniform vec2 somaCenter;\n" 
    "uniform float somaRadius;\n" 
    "void main()\n"
    "{\n"
    "   // Bypass SDF blending for core structures (Soma, Nucleus)\n"
    "   if (somaRadius <= 0.01) {\n"
    "       FragColor = objectColor;\n"
    "   } else {\n"
    "       // Execute SDF blending for Dendrites\n"
    "       float dist = length(FragPos - somaCenter);\n"
    "       \n"
    "       // Fade out the sharp corners inside the soma, keep branches solid outside\n"
    "       float innerEdge = somaRadius - 0.02;\n"
    "       float outerEdge = somaRadius + 0.01;\n"
    "       \n"
    "       // Interpolates from 0.0 (hidden base) to 1.0 (solid branch)\n"
    "       float alphaBlend = smoothstep(innerEdge, outerEdge, dist);\n"
    "       \n"
    "       FragColor = vec4(objectColor.rgb, objectColor.a * alphaBlend);\n"
    "   }\n"
    "}\n";


struct TurtleState {
    glm::vec2 position;
    float angle; // in radians
    float thickness;
};

// Owns the GPU memory and the geometric payload
struct RenderableShape {
    // Allocate Memory (Define Vertices) & Record the State
    unsigned int VAO = 0, VBO = 0;
    std::vector<glm::vec3> vertices;
    float M_PI = std::numbers::pi_v<float>;
    // Mathematical generator on the CPU side
    // Generates a perfect circle if radiusX == radiusY, or an oval if they differ.
    void generateCurvilinearShape(float radiusX, float radiusY, int segments) {
        vertices.clear();
        
        // Origin Point for the GL_TRIANGLE_FAN
        vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); 

        // Circunference points
        float angleStep = (2.0f * M_PI) / segments;
        for (int i = 0; i <= segments; i++) {
            float currentAngle = i * angleStep;
            float x = radiusX * cos(currentAngle);
            float y = radiusY * sin(currentAngle);
            vertices.push_back(glm::vec3(x, y, 0.0f));
        }
    }
    /* memory allocator on the GPU Side which needs the OpenGL context (GLFW window)
    to avoid a fatal crash */
    void allocateVram() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Extracting raw bytes and pointers from the C++ std::vector
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Execution
    void Draw(GLenum drawingMode, unsigned int shaderProgram, float r, float g, float b, float a, glm::vec2 sCenter = glm::vec2(0.0f, 0.0f), float sRadius = 0.0f) {
        // Standard color payload
        int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        glUniform4f(colorLoc, r, g, b, a);

        // Spatial data payload (For SDF Blending in Fragment Shader)
        int centerLoc = glGetUniformLocation(shaderProgram, "somaCenter");
        glUniform2f(centerLoc, sCenter.x, sCenter.y);

        int radiusLoc = glGetUniformLocation(shaderProgram, "somaRadius");
        glUniform1f(radiusLoc, sRadius);

        // Hardware draw call
        glBindVertexArray(VAO);
        glDrawArrays(drawingMode, 0, vertices.size());
        glBindVertexArray(0); // Unbind to protect state
    }

    // Destructor to deallocate memory
        ~RenderableShape() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

};

// The Derived Classes: They inherit VAO, VBO, and vertices 
struct Nucleus : public RenderableShape {
    Nucleus() {
        float radiusX = 0.1f; // narrower width
        float radiusY = 0.15f; // taller height for an oval shape
        int visualFidelity = 36;
        generateCurvilinearShape(radiusX, radiusY, visualFidelity);
    }
};

struct Soma : public RenderableShape {
    // Constructor to define the shape
    Soma() {
        float radius = 0.3f; // equal radius for a circle
        int visualFidelity = 36; // 36 segments for a smooth circle
        generateCurvilinearShape(radius, radius, visualFidelity);
    }
};

struct Dendrite : public RenderableShape {
    // Added numPrimaryDendrites and somaRadius to the parameters
    void generateFractalTopology(int iterations, float somaRadius, int numPrimaryDendrites, float branchLength, float branchAngle, float baseThickness) {

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
            
            // Calculate starting position on the soma's perimeter
            float theta = (2.0f * (float)std::numbers::pi * t) / numPrimaryDendrites;
            
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
                        if (nextThickness < 0.003f) nextThickness = 0.003f;
                        
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
                    currentState.thickness *= 0.65f; // Adjusted taper to survive longer branches
                } else if (c == ']') {
                    currentState = memoryStack.top();
                    memoryStack.pop();
                }
            }
        }
    }
};

// TODO
struct Axon : public RenderableShape {};

// The Manager Class which owns the biology and the state
struct Neuron {
    glm::vec2 position; 

    // LIF Model State Variables (For later use)
    float membrane_potential = -65.0f;
    float resting_potential = -65.0f;
    float threshold = -50.0f;

    // Composition: The Neuron HAS these structural parts
    Soma soma;
    Nucleus nucleus;
    Dendrite dendrite;
    Axon axon;

    // Called once from main() after window creation
    void initializeHardware() {
        soma.allocateVram();
        nucleus.allocateVram();
        dendrite.generateFractalTopology(4, 0.3f, 7, 0.08f, 22.0f, 0.35f);
        dendrite.allocateVram();
        // axon.allocateVram(); 
        
    }

    // Called every frame inside the render loop
    void Draw(unsigned int shaderProgram) {
        soma.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f);
        nucleus.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.0f, 0.38f, 1.0f);
        dendrite.Draw(GL_TRIANGLES, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f, glm::vec2(0.0f, 0.0f), 0.3f);
    }
};

int main() {
    
    

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Neuron Simulation", NULL, NULL);
    if (!window) { 
        glfwTerminate(); 
        return -1; 
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // Add transparency processing to the OpenGl state machine
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Build the Pipeline (Compile & Link Shaders)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define the Orthographic Projection Matrix to replace the distortion of the normalized device coordinates (NDC)
    glUseProgram(shaderProgram);
    float aspectRatio = 800.0f / 600.0f;
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //Scope for objects
    {
        // CPU side
        Neuron myNeuron;
        //GPU side
        myNeuron.initializeHardware();

        // Execution (The Render Loop)
        while (!glfwWindowShouldClose(window)) {
            /*graphics debugging
            glUseProgram(shaderProgram);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            */

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);
            // The entire biological drawing process is triggered by one command
            myNeuron.Draw(shaderProgram);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    

    // Resource Deallocation
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}