#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

Renderer::Renderer(int width, int height, const std::string& title)
    : width(width), height(height) {
    
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    setupOpenGL();
}

Renderer::~Renderer() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void Renderer::setupOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    float fov = 45.0f * 3.14159f / 180.0f;
    float f = 1.0f / std::tan(fov / 2.0f);
    float near = 0.1f, far = 60.0f;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool Renderer::isWindowOpen() const {
    return !glfwWindowShouldClose(window);
}

void Renderer::pollEvents() {
    glfwPollEvents();
}

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

void Renderer::drawScene(RigidBody& body, double cam_dist, double cam_angle_x, double cam_angle_y) {
    clearScreen();
    
    // Camera setup
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    gluPerspective(45.0f, aspect, 0.1f, 60.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    double cx = cam_dist * std::sin(cam_angle_y * M_PI / 180.0) * std::cos(cam_angle_x * M_PI / 180.0);
    double cy = cam_dist * std::sin(cam_angle_x * M_PI / 180.0);
    double cz = cam_dist * std::cos(cam_angle_y * M_PI / 180.0) * std::cos(cam_angle_x * M_PI / 180.0);
    
    gluLookAt(cx, cy, cz, 0, 0, 0, 0, 1, 0);
    
    drawGrid();
    drawAxes();
    drawBody(body);
    
    swapBuffers();
}

void Renderer::drawGrid() {
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.2f, 0.3f);
    glBegin(GL_LINES);
    for (int i = -5; i <= 5; ++i) {
        glVertex3f(i, -2.5f, -5); glVertex3f(i, -2.5f, 5);
        glVertex3f(-5, -2.5f, i); glVertex3f(5, -2.5f, i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void Renderer::drawAxes() {
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    
    glColor3f(1, 0, 0);
    glVertex3f(-3, 0, 0); glVertex3f(3, 0, 0);
    
    glColor3f(0, 1, 0);
    glVertex3f(0, -3, 0); glVertex3f(0, 3, 0);
    
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, -3); glVertex3f(0, 0, 3);
    
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void Renderer::drawBody(RigidBody& body) {
    glPushMatrix();
    
    const auto& vertices = body.getVertices();
    const auto& faces = body.getFaces();
    const auto& color = body.getColor();
    Matrix3d R = body.getRotationMatrix();
    
    if (body.getDrawMethod() == "faces") {
        glBegin(GL_QUADS);
        for (size_t i = 0; i < faces.size(); ++i) {
            const auto& face = faces[i];
            float shade = 0.6f + ((i % 8) * 0.05f);
            glColor3f(color.x * shade, color.y * shade, color.z * shade);
            
            for (int v_idx : face.vertices) {
                auto v = vertices[v_idx];
                Eigen::Vector3d eigen_v(v.x, v.y, v.z);
                Eigen::Vector3d rotated = R * eigen_v;
                glVertex3f(rotated(0), rotated(1), rotated(2));
            }
        }
        glEnd();
    }
    
    // Draw trails
    glDisable(GL_LIGHTING);
    const auto& trail_top = body.getTrailTop();
    const auto& trail_bottom = body.getTrailBottom();
    
    if (trail_top.size() > 1) {
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < trail_top.size(); ++i) {
            float alpha = (float)i / trail_top.size();
            glColor4f(1, 1, 0, alpha);
            glVertex3f(trail_top[i](0), trail_top[i](1), trail_top[i](2));
        }
        glEnd();
        
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < trail_bottom.size(); ++i) {
            float alpha = (float)i / trail_bottom.size();
            glColor4f(1, 0.5f, 0, alpha);
            glVertex3f(trail_bottom[i](0), trail_bottom[i](1), trail_bottom[i](2));
        }
        glEnd();
        glLineWidth(1.0f);
    }
    
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
