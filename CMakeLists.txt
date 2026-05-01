cmake_minimum_required(VERSION 3.21)
project(NeuronsSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(glm CONFIG REQUIRED)

if(EMSCRIPTEN)
    set(DEPS glm::glm)
else()
    find_package(glad CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)

    set(DEPS glfw glad::glad glm::glm OpenGL::GL)
endif()

# 2D Topology executable
# 2D Topology executable
add_executable(Neurons2D 
    src/main.cpp 
    src/RenderableShape.cpp 
    src/CellularMorphology.cpp 
    src/Neuron.cpp
    src/NeuralNetworkManager.cpp
)
target_link_libraries(Neurons2D PRIVATE ${DEPS})
target_include_directories(Neurons2D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(EMSCRIPTEN)
    set_target_properties(Neurons2D PROPERTIES SUFFIX ".js")
    target_compile_options(Neurons2D PRIVATE --bind)
    target_link_options(Neurons2D PRIVATE
        --bind
        "SHELL:-s USE_WEBGL2=1"
        "SHELL:-s USE_GLFW=3"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s EXPORT_NAME='createNeuralEngine'"
        "SHELL:-s MODULARIZE=1"
    )
endif()
