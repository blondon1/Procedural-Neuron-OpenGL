#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <numbers>

// Owns the GPU memory and the geometric payload
class RenderableShape {
private:
    // Allocate Memory (Define Vertices) & Record the State
    unsigned int VAO = 0, VBO = 0;
    float M_PI = std::numbers::pi_v<float>;

protected:
    std::vector<glm::vec3> vertices;
    void generateCurvilinearShape(float radiusX, float radiusY, int segments);

public:
    void allocateVram();
    void Draw(GLenum drawingMode, unsigned int shaderProgram, float r, float g, float b, float a, glm::vec2 sCenter = glm::vec2(0.0f, 0.0f), float sRadius = 0.0f);
    ~RenderableShape();

};