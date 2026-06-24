#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../include/rigid_body.hpp"

extern void initRenderer();
extern void setupCamera(double, double, double, int, int);
extern void renderStars();
extern void renderGrid();
extern void renderAxes();
extern void renderVectors(const RigidBody&);
extern void renderRigidBody(const RigidBody&);

RigidBody* g_body = nullptr;
bool g_paused = false;

void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "KEY:" << key << std::endl;
        if (key == GLFW_KEY_1) { delete g_body; g_body = new RigidBody(BodyShape::BORBOLETA); std::cout << "Borboleta" << std::endl; }
        if (key == GLFW_KEY_2) { delete g_body; g_body = new RigidBody(BodyShape::RAQUETE); std::cout << "Raquete" << std::endl; }
        if (key == GLFW_KEY_3) { delete g_body; g_body = new RigidBody(BodyShape::BOLA); std::cout << "Bola" << std::endl; }
        if (key == GLFW_KEY_SPACE) { g_paused = !g_paused; }
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, GLFW_TRUE);
    }
}

int main() {
    std::cout << "Dzhanibekov - OpenGL MODERNO!" << std::endl;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    GLFWwindow* w = glfwCreateWindow(900, 600, "Dzhanibekov OpenGL 3.3", NULL, NULL);
    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);
    glfwSetKeyCallback(w, key_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD failed!" << std::endl;
        return -1;
    }
    
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Estrelas FUNCIONANDO!" << std::endl;
    
    initRenderer();
    g_body = new RigidBody(BodyShape::BORBOLETA);
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    
    int frame = 0;
    while (!glfwWindowShouldClose(w)) {
        if (!g_paused) g_body->update(0.016);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        setupCamera(8.0, 25.0, 45.0, 900, 600);
        renderStars();  // 🌟 ESTRELAS!
        renderGrid();
        renderAxes();
        renderRigidBody(*g_body);
        renderVectors(*g_body);
        
        glfwSwapBuffers(w);
        glfwPollEvents();
        
        frame++;
        if (frame % 180 == 0) std::cout << "Frame " << frame << std::endl;
    }
    
    delete g_body;
    glfwTerminate();
    return 0;
}

