cmake_minimum_required(VERSION 3.21)
project(NeuronsSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Common dependencies
set(DEPS glfw glad::glad glm::glm OpenGL::GL)

# 2D Topology executable
# 2D Topology executable
add_executable(Neurons2D 
    src/main.cpp 
    src/RenderableShape.cpp 
    src/CellularMorphology.cpp 
    src/Neuron.cpp
)
target_link_libraries(Neurons2D PRIVATE ${DEPS})
target_include_directories(Neurons2D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

