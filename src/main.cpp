#include "CellularMorphology.h"
#include "Neuron.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Define Shader Source Code (GLSL - C like)
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 model;\n" // INJECTED: The spatial translation matrix
    "out vec2 FragPos;\n" 
    "void main()\n"
    "{\n"
    "   // Multiply local vertex by model matrix, then projection lens\n"
    "   gl_Position = projection * model * vec4(aPos, 1.0);\n" 
    "   FragPos = vec2(aPos.x, aPos.y);\n" // Kept local to preserve SDF blending
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
    float cameraZoom = 2.5f; // Expanded to encompass the network
    float cameraPanX = 1.6f; // Shifted to frame the right-facing structure
    glm::mat4 projection = glm::ortho(
        (-aspectRatio * cameraZoom) + cameraPanX, 
        (aspectRatio * cameraZoom) + cameraPanX, 
        -cameraZoom, 
        cameraZoom, 
        -1.0f, 1.0f
    );
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //Scope for objects
    {
        // CPU SIDE: INSTANTIATION
        // We delete 'myNeuron' and spawn two distinct biological agents
        Neuron presynapticNeuron;
        Neuron postsynapticNeuron;
        
        // Offset the target neuron 3.2 units to the right
        postsynapticNeuron.SetPosition(3.2f, 0.0f);

        // GPU SIDE: BUFFER ALLOCATION
        // Both distinct objects must generate their geometry on the graphics card
        presynapticNeuron.initializeHardware();
        postsynapticNeuron.initializeHardware();

        // TOPOLOGICAL WIRING
        // Connect presynaptic to postsynaptic.
        // We set the weight to a massive 16.0f mV. Because the target rests at -70mV 
        // and triggers at -55mV, this single injection will instantly force it to fire.
        presynapticNeuron.AddSynapse(&postsynapticNeuron, 16.0f);

        // Temporal State
        float deltaTime = 0.0f;

        // TEMPORAL STATE
        float lastFrame = 0.0f;  

        // EXECUTION (THE RENDER LOOP)
        while (!glfwWindowShouldClose(window)) {

            // HARDWARE INTERRUPT:
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                // We STRICTLY only stimulate the left neuron to prove they are isolated
                presynapticNeuron.InjectStimulus(8.0f); 
                std::cout << "[HARDWARE] SPACEBAR PRESSED!\n"; 
            }

            // Calculate the rigid Delta Time 
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // PHYSICS MECHANICS: Update BOTH discrete state machines
            presynapticNeuron.Update(deltaTime);
            postsynapticNeuron.Update(deltaTime);

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);

            // RENDER DISPATCH: Draw BOTH objects using the new spatial matrix
            presynapticNeuron.Draw(shaderProgram);
            postsynapticNeuron.Draw(shaderProgram);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
    // Resource Deallocation
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;

}