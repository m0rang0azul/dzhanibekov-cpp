#include "simulation.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>

Simulation::Simulation(int width, int height)
    : renderer(std::make_unique<Renderer>(width, height, "Dzhanibekov - Physically Accurate")),
      paused(false), speed(1.0), gravity_enabled(false), gravity_accel(0.0),
      frame_count(0), flip_count(0), flip_cooldown(0),
      cam_dist(8), cam_angle_x(25), cam_angle_y(45) {
    
    body = std::make_unique<RigidBody>(BodyShape::BORBOLETA);
    prev_y = Vector3d(0, 1, 0);
}

Simulation::~Simulation() = default;

bool Simulation::handleInput() {
    if (!renderer->isWindowOpen()) {
        return false;
    }
    
    renderer->pollEvents();
    
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return false;
    }
    
    // Shape selection
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_1) == GLFW_PRESS) {
        body = std::make_unique<RigidBody>(BodyShape::BORBOLETA);
        flip_count = 0;
        prev_y = Vector3d(0, 1, 0);
        flip_cooldown = 0;
    }
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_2) == GLFW_PRESS) {
        body = std::make_unique<RigidBody>(BodyShape::RAQUETE);
        flip_count = 0;
        prev_y = Vector3d(0, 1, 0);
        flip_cooldown = 0;
    }
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_3) == GLFW_PRESS) {
        body = std::make_unique<RigidBody>(BodyShape::BOLA);
        flip_count = 0;
        prev_y = Vector3d(0, 1, 0);
        flip_cooldown = 0;
    }
    
    // Gravity
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_I) == GLFW_PRESS) {
        gravity_enabled = true;
        gravity_accel = 0.001;
    }
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
        gravity_enabled = true;
        gravity_accel = 9.81;
    }
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_0) == GLFW_PRESS) {
        gravity_enabled = false;
        gravity_accel = 0.0;
    }
    
    // Pause/Resume
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        paused = !paused;
    }
    
    // Reset
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_R) == GLFW_PRESS) {
        body = std::make_unique<RigidBody>(body->getShape());
        flip_count = 0;
        prev_y = Vector3d(0, 1, 0);
        flip_cooldown = 0;
    }
    
    // Speed
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
        speed = std::min(3.0, speed + 0.01);
    }
    if (glfwGetKey(renderer->getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
        speed = std::max(0.25, speed - 0.01);
    }
    
    // Mouse camera control
    static double last_x = 0, last_y = 0;
    double x, y;
    glfwGetCursorPos(renderer->getWindow(), &x, &y);
    
    if (glfwGetMouseButton(renderer->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        cam_angle_y += (x - last_x) * 0.3;
        cam_angle_x += (y - last_y) * 0.3;
    }
    
    last_x = x;
    last_y = y;
    
    return true;
}

void Simulation::update(double dt) {
    if (!paused) {
        body->update(dt * speed, gravity_enabled, gravity_accel);
        detectFlip();
    }
}

void Simulation::detectFlip() {
    if (flip_cooldown > 0) {
        flip_cooldown--;
        return;
    }
    
    Matrix3d R = body->getRotationMatrix();
    Vector3d current_y = R * Vector3d(0, 1, 0);
    double dot = current_y.dot(prev_y);
    
    if (dot < -0.5) {
        flip_count++;
        flip_cooldown = 90;
        std::cout << "⚡ FLIP #" << flip_count << "!\n";
        prev_y = current_y;
    }
    else if (dot > 0.5) {
        prev_y = current_y;
    }
}

void Simulation::run() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "Without gravity: L and E CONSERVED\n";
    std::cout << "With gravity: external torque\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Controls: 1,2,3 | I,E,0 | ↑↓ | SPACE | R | Mouse\n";
    std::cout << std::string(50, '=') << "\n\n";
    
    auto last_time = std::chrono::high_resolution_clock::now();
    
    while (handleInput()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(current_time - last_time).count();
        last_time = current_time;
        
        frame_count++;
        
        update(dt);
        renderer->drawScene(*body, cam_dist, cam_angle_x, cam_angle_y);
        
        if (frame_count % 180 == 0) {
            double E = body->getEnergy();
            double L = body->getAngularMomentumMagnitude();
            Vector3d omega = body->getAngularVelocity();
            
            std::cout << std::fixed << std::setprecision(3);
            std::cout << "⏱️  Frame " << frame_count 
                      << " | Flips: " << flip_count
                      << " | E=" << E << "J"
                      << " | L=" << L
                      << " | Speed: " << speed << "x\n";
        }
    }
    
    std::cout << "\n👋 Simulation closed\n";
}
