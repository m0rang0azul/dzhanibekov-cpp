#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "rigid_body.hpp"
#include "renderer.hpp"
#include <memory>

class Simulation {
private:
    std::unique_ptr<RigidBody> body;
    std::unique_ptr<Renderer> renderer;
    
    bool paused;
    double speed;
    bool gravity_enabled;
    double gravity_accel;
    
    int frame_count;
    int flip_count;
    Vector3d prev_y;
    int flip_cooldown;
    
    // Camera
    double cam_dist;
    double cam_angle_x;
    double cam_angle_y;
    
public:
    Simulation(int width = 900, int height = 600);
    ~Simulation();
    
    void run();
    
private:
    bool handleInput();
    void update(double dt);
    void detectFlip();
};

#endif
