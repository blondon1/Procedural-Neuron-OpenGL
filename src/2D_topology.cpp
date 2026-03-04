#include "CellularMorphology.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
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

// The Manager Class which owns the biology and the state
class Neuron {
private:
    glm::vec2 position; 

    // LIF Model State Variables (For later use)
    float membrane_potential = -65.0f;
    float resting_potential = -65.0f;
    float threshold = -50.0f;

    // Composition: The Neuron HAS these structural parts
    Soma soma;
    Cytoplasm cytoplasm;
    Nucleus nucleus;
    Dendrite dendrite;
    Axon axon;
public:
    // Called once from main() after window creation
    void initializeHardware() {
        soma.allocateVram();
        cytoplasm.allocateVram();
        nucleus.allocateVram();
        dendrite.generateFractalTopology(4, 0.3f, 7, 0.08f, 22.0f, 0.35f);
        dendrite.allocateVram();
        // axon.allocateVram(); 
        
    }

    // Called every frame inside the render loop
    void Draw(unsigned int shaderProgram) {
        dendrite.Draw(GL_TRIANGLES, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f, glm::vec2(0.0f, 0.0f), 0.0f);
        soma.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f);
        cytoplasm.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.15f, 0.75f, 1.0f, 1.0f);
        nucleus.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.0f, 0.38f, 1.0f);
        
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