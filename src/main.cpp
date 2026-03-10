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
    float cameraZoom = 1.1f;
    float cameraPanX = 0.7f;
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
        // CPU side
        Neuron myNeuron;
        //GPU side
        myNeuron.initializeHardware();

        // Temporal State
        float deltaTime = 0.0f;  // The biological time passed between the current and previous frame
        float lastFrame = 0.0f;  // The absolute timestamp of the previous frame

        // Execution (The Render Loop)
        while (!glfwWindowShouldClose(window)) {
            /*graphics debugging
            glUseProgram(shaderProgram);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            */

            // Holding the spacebar pumps positive current into the dendrites
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                // Inject a steady stream of voltage per frame
                myNeuron.InjectStimulus(8.0f); 
                std::cout << "[HARDWARE] SPACEBAR PRESSED!\n"; 
            }

            // Calculate the rigid Delta Time for the physics state machine
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // std::cout << "Delta Time: " << deltaTime << " seconds\n";



            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);

            myNeuron.Update(deltaTime);

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