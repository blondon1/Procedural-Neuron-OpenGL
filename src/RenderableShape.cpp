#include "RenderableShape.h"

// Mathematical generator on the CPU side
// Generates a perfect circle if radiusX == radiusY, or an oval if they differ.
void RenderableShape::generateCurvilinearShape(float radiusX, float radiusY, int segments) {
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
void RenderableShape::allocateVram() {
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
void RenderableShape::Draw(GLenum drawingMode, unsigned int shaderProgram, float r, float g, float b, float a, glm::vec2 sCenter, float sRadius) {
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
    RenderableShape::~RenderableShape() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}