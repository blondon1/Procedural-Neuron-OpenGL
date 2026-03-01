#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <numbers>
#define _USE_MATH_DEFINES

// Define Shader Source Code (GLSL - C like)
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 objectColor;\n" 
    "void main()\n"
    "{\n"
    "   FragColor = objectColor;\n" 
    "}\n";


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
    void Draw(GLenum drawingMode, unsigned int shaderProgram, float r, float g, float b, float a) {
        int location = glGetUniformLocation(shaderProgram, "objectColor");
        glUniform4f(location, r, g, b, a);
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

// TODO
struct Dendrite : public RenderableShape {};
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
        // axon.allocateVram(); 
        // dendrite.allocateVram();
    }

    // Called every frame inside the render loop
    void Draw(unsigned int shaderProgram) {
        // Draw Soma: Dark Blue (R: 0.1, G: 0.2, B: 0.5, A: 1.0)
        soma.Draw(GL_TRIANGLE_FAN, shaderProgram, 0.0f, 0.651f, 1.0f, 1.0f);
        
        // Draw Nucleus: Light Grey/Yellow (R: 0.8, G: 0.8, B: 0.7, A: 1.0)
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