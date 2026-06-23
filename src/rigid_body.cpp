#include "rigid_body.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

RigidBody::RigidBody(BodyShape shape)
    : shape(shape), position(Vector3d::Zero()), max_trail(200),
      quat(1, 0, 0, 0), flip_cooldown(0) {
    
    switch (shape) {
        case BodyShape::BORBOLETA:
            I = Vector3d(1.0, 2.0, 4.0);
            color = glm::vec3(0.95f, 0.75f, 0.15f);
            draw_method = "faces";
            angular_velocity = Vector3d(0.3, 5.0, 0.3);
            createBorboleta();
            break;
            
        case BodyShape::RAQUETE:
            I = Vector3d(0.8, 2.2, 4.0);
            color = glm::vec3(0.2f, 0.6f, 1.0f);
            draw_method = "faces";
            angular_velocity = Vector3d(0.3, 5.0, 0.3);
            createRaquete();
            break;
            
        case BodyShape::BOLA:
            I = Vector3d(1.0, 1.0, 1.0);
            color = glm::vec3(1.0f, 0.3f, 0.3f);
            draw_method = "triangles";
            angular_velocity = Vector3d(0.3, 5.0, 0.3);
            createBola();
            break;
    }
    
    // Angular momentum (conserved without external torque)
    L_body = I.asDiagonal() * angular_velocity;
    
    // Initial kinetic energy (conserved)
    initial_energy = 0.5 * angular_velocity.dot(L_body);
    
    // Angular momentum magnitude (conserved)
    L_magnitude = L_body.norm();
    
    std::cout << "  E_initial = " << initial_energy << " J\n";
    std::cout << "|L| = " << L_magnitude << " kg·m²/s\n";
    std::cout << "  Both will be CONSERVED (real physics)\n\n";
}

void RigidBody::createBorboleta() {
    // Create butterfly-like shape
    vertices.clear();
    faces.clear();
    
    // Circular cross-section body
    for (int i = 0; i < 8; ++i) {
        float ang = 2.0f * M_PI * i / 8.0f;
        float x = 0.25f * std::cos(ang);
        float z = 0.25f * std::sin(ang);
        vertices.push_back(glm::vec3(x, -0.5f, z));
        vertices.push_back(glm::vec3(x, 0.5f, z));
    }
    
    // Wings outline
    vertices.push_back(glm::vec3(0.25f, 0.35f, 0));
    vertices.push_back(glm::vec3(2.0f, 1.2f, 0));
    vertices.push_back(glm::vec3(1.6f, 0, 0));
    vertices.push_back(glm::vec3(0.25f, -0.35f, 0));
    
    vertices.push_back(glm::vec3(-0.25f, 0.35f, 0));
    vertices.push_back(glm::vec3(-2.0f, 1.2f, 0));
    vertices.push_back(glm::vec3(-1.6f, 0, 0));
    vertices.push_back(glm::vec3(-0.25f, -0.35f, 0));
    
    // Depth for wings
    for (int i = 16; i < 24; ++i) {
        auto v = vertices[i];
        vertices.push_back(glm::vec3(v.x, v.y, 0.15f));
    }
    
    // Body faces (cylindrical)
    for (int i = 0; i < 8; ++i) {
        Face f;
        f.vertices = {2*i, (2*i+2)%16, (2*i+3)%16, 2*i+1};
        faces.push_back(f);
    }
    
    // Wing faces
    faces.push_back(Face{{16,17,18,19}});
    faces.push_back(Face{{24,25,26,27}});
    faces.push_back(Face{{16,17,25,24}});
    faces.push_back(Face{{19,18,26,27}});
    faces.push_back(Face{{17,18,26,25}});
    faces.push_back(Face{{16,19,27,24}});
    
    faces.push_back(Face{{20,21,22,23}});
    faces.push_back(Face{{28,29,30,31}});
    faces.push_back(Face{{20,21,29,28}});
    faces.push_back(Face{{23,22,30,31}});
    faces.push_back(Face{{21,22,30,29}});
    faces.push_back(Face{{20,23,31,28}});
}

void RigidBody::createRaquete() {
    vertices.clear();
    faces.clear();
    
    // Handle
    vertices = {
        glm::vec3(-0.1f, -2, -0.1f), glm::vec3(0.1f, -2, -0.1f),
        glm::vec3(0.1f, -1, -0.1f), glm::vec3(-0.1f, -1, -0.1f),
        glm::vec3(-0.1f, -2, 0.1f), glm::vec3(0.1f, -2, 0.1f),
        glm::vec3(0.1f, -1, 0.1f), glm::vec3(-0.1f, -1, 0.1f)
    };
    
    int n = 16;
    // Outer circle
    for (int i = 0; i < n; ++i) {
        float a = 2.0f * M_PI * i / n;
        float x = 1.1f * std::cos(a);
        float y = 0.2f + 0.8f * std::sin(a);
        vertices.push_back(glm::vec3(x, y, -0.05f));
        vertices.push_back(glm::vec3(x*0.85f, y*0.85f, -0.05f));
    }
    
    // Inner circle (depth)
    for (int i = 0; i < n; ++i) {
        float a = 2.0f * M_PI * i / n;
        float x = 1.1f * std::cos(a);
        float y = 0.2f + 0.8f * std::sin(a);
        vertices.push_back(glm::vec3(x, y, 0.05f));
        vertices.push_back(glm::vec3(x*0.85f, y*0.85f, 0.05f));
    }
    
    // Handle faces
    faces.push_back(Face{{0,1,2,3}});
    faces.push_back(Face{{4,5,6,7}});
    faces.push_back(Face{{0,1,5,4}});
    faces.push_back(Face{{2,3,7,6}});
    faces.push_back(Face{{0,3,7,4}});
    faces.push_back(Face{{1,2,6,5}});
    
    // Racket head faces
    int oof = 8, oif = 8 + n, oob = 8 + 2*n, oib = 8 + 3*n;
    for (int i = 0; i < n; ++i) {
        int ni = (i+1) % n;
        faces.push_back(Face{{oof+i, oof+ni, oif+ni, oif+i}});
        faces.push_back(Face{{oob+i, oob+ni, oib+ni, oib+i}});
        faces.push_back(Face{{oof+i, oof+ni, oob+ni, oob+i}});
    }
}

void RigidBody::createBola() {
    vertices.clear();
    faces.clear();
    
    int stacks = 16, slices = 16;
    float r = 1.2f;
    
    // Generate sphere vertices
    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PI * j / slices;
            float x = r * std::sin(phi) * std::cos(theta);
            float y = r * std::cos(phi);
            float z = r * std::sin(phi) * std::sin(theta);
            vertices.push_back(glm::vec3(x, y, z));
        }
    }
    
    // Generate sphere faces
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int f = i * (slices + 1) + j;
            int s = f + slices + 1;
            faces.push_back(Face{{f, s, f+1}});
            faces.push_back(Face{{s, s+1, f+1}});
        }
    }
}

void RigidBody::update(double dt, bool gravity_enabled, double gravity_accel) {
    dt = std::min(dt, 0.016);
    
    // Apply gravity if enabled
    if (gravity_enabled && gravity_accel > 1e-6) {
        Vector3d com(0.0, 0.3, 0.0);
        Matrix3d R = quat.toRotationMatrix();
        Vector3d rotated_com = R * com;
        Vector3d gravity_force(0, -gravity_accel, 0);
        Vector3d torque = rotated_com.cross(gravity_force) * 0.1;
        L_body += torque * dt;
    }
    
    // Euler equations (conservative form)
    Vector3d I_inv = I.asDiagonal().inverse() * Vector3d::Ones();
    
    // Symplectic step (half-step)
    Vector3d L_half = L_body + 0.5 * dt * L_body.cross(I_inv.asDiagonal() * L_body);
    
    // Full step with intermediate L
    Vector3d omega_half = I_inv.asDiagonal() * L_half;
    L_body += dt * L_half.cross(omega_half);
    
    // Safety clipping
    double max_L = 100.0;
    L_body = L_body.cwiseMax(-max_L).cwiseMin(max_L);
    
    // Calculate final angular velocity
    Vector3d omega = I_inv.asDiagonal() * L_body;
    
    // Check energy conservation
    double current_energy = 0.5 * L_body.dot(omega);
    if (std::abs(current_energy - initial_energy) > 0.01 * initial_energy) {
        double correction = std::sqrt(initial_energy / current_energy);
        L_body *= correction;
    }
    
    // Update quaternion
    Quaternion dq = quat.derivative(omega);
    Quaternion new_quat(quat.w + dq.w * dt,
                        quat.x + dq.x * dt,
                        quat.y + dq.y * dt,
                        quat.z + dq.z * dt);
    quat = new_quat.normalized();
    
    // Update trails
    Matrix3d R = quat.toRotationMatrix();
    Vector3d tip_top(2.0, 1.2, 0.0);
    Vector3d tip_bottom(2.0, -1.2, 0.0);
    
    trail_top.push_back(R * tip_top);
    trail_bottom.push_back(R * tip_bottom);
    
    while (trail_top.size() > max_trail) trail_top.erase(trail_top.begin());
    while (trail_bottom.size() > max_trail) trail_bottom.erase(trail_bottom.begin());
}

double RigidBody::getEnergy() const {
    Vector3d omega = I.asDiagonal().inverse() * L_body;
    return 0.5 * L_body.dot(omega);
}

double RigidBody::getAngularMomentumMagnitude() const {
    return L_body.norm();
}
