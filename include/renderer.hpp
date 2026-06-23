#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "rigid_body.hpp"
#include <memory>

class Renderer {
private:
    GLFWwindow* window;
    int width, height;
    
public:
    Renderer(int width, int height, const std::string& title);
    ~Renderer();
    
    bool isWindowOpen() const;
    void pollEvents();
    void clearScreen();
    void drawScene(RigidBody& body, double cam_dist, double cam_angle_x, double cam_angle_y);
    void swapBuffers();
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    GLFWwindow* getWindow() const { return window; }
    
private:
    void setupOpenGL();
    void drawGrid();
    void drawAxes();
    void drawBody(RigidBody& body);
};

#endif
