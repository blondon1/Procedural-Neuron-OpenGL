#include "CellularMorphology.h"
#include "NeuralNetworkManager.h"
#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#endif
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

#ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
#endif

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

    // CAMERA STATE VARIABLES
    glUseProgram(shaderProgram);
    float aspectRatio = 800.0f / 600.0f;
    float cameraZoom = 6.0f;  
    float cameraPanX = 0.0f;  // Centered mathematically
    float cameraPanY = 0.0f;  // Y-axis translation
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Scope for objects
    {
        // --- 1. THE NEURAL GRAPH CONTROLLER (INITIALIZE ONCE) ---
        NeuralNetworkManager neuralGraph;
        neuralGraph.InitializeProceduralGraph(1000);

        // --- 2. TEMPORAL STATE ---
        constexpr float fixedDt = 0.01666f;
        float accumulator = 0.0f;
        float lastFrame = static_cast<float>(glfwGetTime());

        // --- 3. EXECUTION (THE AUTONOMOUS RENDER LOOP) ---
        while (!glfwWindowShouldClose(window)) {

            // CHRONOMETER & TEMPORAL DILATION
            float currentFrame = static_cast<float>(glfwGetTime());
            float frameTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            accumulator += frameTime;

            // HARDWARE INTERRUPTS (THE CAMERA)
            // Zoom (Up/Down Arrows)
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                cameraZoom -= 2.0f * frameTime; 
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                cameraZoom += 2.0f * frameTime; 
            }

            // Planar Translation (WASD)
            // Mathematically bind pan speed to the zoom level for consistent optical tracking
            float panSpeed = 1.0f * cameraZoom; 
            
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                cameraPanY += panSpeed * frameTime; // Move up
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                cameraPanY -= panSpeed * frameTime; // Move down
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                cameraPanX -= panSpeed * frameTime; // Move left
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                cameraPanX += panSpeed * frameTime; // Move right
            }

            // DYNAMIC PROJECTION MATRIX
            glm::mat4 projection = glm::ortho(
                (-aspectRatio * cameraZoom) + cameraPanX, 
                (aspectRatio * cameraZoom) + cameraPanX, 
                -cameraZoom + cameraPanY, // Injected Y-Axis bottom boundary
                cameraZoom + cameraPanY,  // Injected Y-Axis top boundary
                -1.0f, 1.0f
            );
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            // THE NEURAL GRAPH CONTROLLER: PHYSICS MECHANICS
            // (Stochastic noise is now handled cleanly inside this Update method)
            while (accumulator >= fixedDt) {
                neuralGraph.Update(fixedDt);
                accumulator -= fixedDt;
            }

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shaderProgram);

            // THE NEURAL GRAPH CONTROLLER: RENDER DISPATCH
            neuralGraph.Draw(shaderProgram);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
    // Resource Deallocation
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;

}
