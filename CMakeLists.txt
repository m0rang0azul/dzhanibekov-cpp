cmake_minimum_required(VERSION 3.20)
project(dzhanibekov LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_USE_MATH_DEFINES -DNOMINMAX)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    C:/Users/luana/libs/eigen
    C:/Users/luana/vcpkg/installed/x64-windows/include
)

link_directories(
    C:/Users/luana/vcpkg/installed/x64-windows/lib
)

add_executable(dzhanibekov
    src/main.cpp
    src/rigid_body.cpp
    src/renderer.cpp
    src/simulation.cpp
    src/geometry.cpp
)

target_link_libraries(dzhanibekov
    opengl32
    glfw3dll
)
