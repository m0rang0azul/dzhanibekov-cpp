#include <windows.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../include/rigid_body.hpp"

extern void renderGrid();
extern void renderAxes();
extern void setupCamera(double, double, double, int, int);
extern void renderRigidBody(const RigidBody&);

// Variáveis globais para o callback
RigidBody* g_body = nullptr;
bool g_paused = false;
int g_frame = 0;

void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "Tecla: " << key << " (" << (char)key << ")" << std::endl;
        
        if (key == GLFW_KEY_1 || key == GLFW_KEY_KP_1) {
            delete g_body;
            g_body = new RigidBody(BodyShape::BORBOLETA);
            std::cout << ">> BORBOLETA <<" << std::endl;
        }
        else if (key == GLFW_KEY_2 || key == GLFW_KEY_KP_2) {
            delete g_body;
            g_body = new RigidBody(BodyShape::RAQUETE);
            std::cout << ">> RAQUETE <<" << std::endl;
        }
        else if (key == GLFW_KEY_3 || key == GLFW_KEY_KP_3) {
            delete g_body;
            g_body = new RigidBody(BodyShape::BOLA);
            std::cout << ">> BOLA <<" << std::endl;
        }
        else if (key == GLFW_KEY_SPACE) {
            g_paused = !g_paused;
            std::cout << (g_paused ? "PAUSADO" : "RODANDO") << std::endl;
        }
        else if (key == GLFW_KEY_R) {
            BodyShape s = g_body->getShape();
            delete g_body;
            g_body = new RigidBody(s);
            std::cout << "RESET" << std::endl;
        }
        else if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(w, GLFW_TRUE);
        }
    }
}

int main() {
    std::cout << "Dzhanibekov - Controles: 1=Borb 2=Raq 3=Bola ESP=Pausa R=Reset ESC=Sair" << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!" << std::endl;
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(900, 600, "Dzhanibekov - 1,2,3", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    
    // Configuração OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    
    // ILUMINAÇÃO CORRETA
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat position[] = {5.0f, 5.0f, 5.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    g_body = new RigidBody(BodyShape::BORBOLETA);
    
    double cam_dist = 8.0, cam_angle_x = 25.0, cam_angle_y = 45.0;
    
    std::cout << "Pressione 1,2,3 para trocar objetos!" << std::endl;
    
    while (!glfwWindowShouldClose(window)) {
        if (!g_paused) {
            g_body->update(0.016);
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        setupCamera(cam_dist, cam_angle_x, cam_angle_y, 900, 600);
        renderGrid();
        renderAxes();
        renderRigidBody(*g_body);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        g_frame++;
        if (g_frame % 180 == 0) {
            std::cout << "Frame " << g_frame << std::endl;
        }
    }
    
    delete g_body;
    glfwTerminate();
    return 0;
}
