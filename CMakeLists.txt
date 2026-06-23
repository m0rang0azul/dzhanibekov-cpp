cmake_minimum_required(VERSION 3.10)
project(DzhanibekovSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

# Dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/rigid_body.cpp
    src/simulation.cpp
    src/renderer.cpp
    src/geometry.cpp
)

# Executable
add_executable(dzhanibekov ${SOURCES})

target_link_libraries(dzhanibekov
    OpenGL::OpenGL
    glfw
    Eigen3::Eigen
)
