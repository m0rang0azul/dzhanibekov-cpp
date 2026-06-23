#include <cmath>
#include "../include/rigid_body.hpp"
#include <iostream>

RigidBody::RigidBody(BodyShape shape) 
    : shape(shape)
    , max_trail(200)
    , position(0.0, 0.0, 0.0)
    , quat(1.0, 0.0, 0.0, 0.0) {
    
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
    
    L_body = Vector3d(I.x() * angular_velocity.x(),
                      I.y() * angular_velocity.y(),
                      I.z() * angular_velocity.z());
    
    initial_energy = 0.5 * (I.x() * angular_velocity.x() * angular_velocity.x() +
                            I.y() * angular_velocity.y() * angular_velocity.y() +
                            I.z() * angular_velocity.z() * angular_velocity.z());
    
    L_magnitude = L_body.norm();
    
    std::cout << "RigidBody criado: " << (shape == BodyShape::BORBOLETA ? "Borboleta" : 
                                           shape == BodyShape::RAQUETE ? "Raquete" : "Bola") << std::endl;
}

// Implementações das criações de geometria (mesmas de antes)
void RigidBody::createBorboleta() {
    vertices.clear();
    faces.clear();
    
    for (int i = 0; i < 8; i++) {
        double ang = 2.0 * 3.14159265358979323846 * i / 8.0;
        float x = 0.25f * cos(ang);
        float z = 0.25f * sin(ang);
        vertices.push_back(glm::vec3(x, -0.5f, z));
        vertices.push_back(glm::vec3(x, 0.5f, z));
    }
    
    vertices.push_back(glm::vec3(0.25f, 0.35f, 0.0f));
    vertices.push_back(glm::vec3(2.0f, 1.2f, 0.0f));
    vertices.push_back(glm::vec3(1.6f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(0.25f, -0.35f, 0.0f));
    
    vertices.push_back(glm::vec3(-0.25f, 0.35f, 0.0f));
    vertices.push_back(glm::vec3(-2.0f, 1.2f, 0.0f));
    vertices.push_back(glm::vec3(-1.6f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(-0.25f, -0.35f, 0.0f));
    
    for (int i = 16; i < 24; i++) {
        vertices.push_back(glm::vec3(vertices[i].x, vertices[i].y, 0.15f));
    }
    
    for (int i = 0; i < 8; i++) {
        faces.push_back({{2*i, (2*i+2)%16, (2*i+3)%16, 2*i+1}});
    }
    
    faces.push_back({{16, 17, 18, 19}});
    faces.push_back({{24, 25, 26, 27}});
    faces.push_back({{16, 17, 25, 24}});
    faces.push_back({{19, 18, 26, 27}});
    faces.push_back({{17, 18, 26, 25}});
    faces.push_back({{16, 19, 27, 24}});
    faces.push_back({{20, 21, 22, 23}});
    faces.push_back({{28, 29, 30, 31}});
    faces.push_back({{20, 21, 29, 28}});
    faces.push_back({{23, 22, 30, 31}});
    faces.push_back({{21, 22, 30, 29}});
    faces.push_back({{20, 23, 31, 28}});
}

// (createRaquete e createBola similares...)

void RigidBody::update(double dt, bool gravity_enabled, double gravity_accel) {
    dt = std::min(dt, 0.016);
    
    if (gravity_enabled && gravity_accel > 1e-6) {
        Vector3d com(0.0, 0.3, 0.0);
        Matrix3d R = quat.toRotationMatrix();
        Vector3d rotated_com = R * com;
        Vector3d gravity_force(0.0, -gravity_accel, 0.0);
        Vector3d torque = rotated_com.cross(gravity_force) * 0.1;
        L_body += torque * dt;
    }
    
    Vector3d omega(L_body.x() / I.x(), L_body.y() / I.y(), L_body.z() / I.z());
    
    double max_omega = 50.0;
    if (omega.x() > max_omega) omega.x() = max_omega;
    if (omega.x() < -max_omega) omega.x() = -max_omega;
    if (omega.y() > max_omega) omega.y() = max_omega;
    if (omega.y() < -max_omega) omega.y() = -max_omega;
    if (omega.z() > max_omega) omega.z() = max_omega;
    if (omega.z() < -max_omega) omega.z() = -max_omega;
    
    Vector3d L_dot = -omega.cross(L_body);
    L_body += L_dot * dt;
    
    double max_L = 100.0;
    if (L_body.x() > max_L) L_body.x() = max_L;
    if (L_body.x() < -max_L) L_body.x() = -max_L;
    if (L_body.y() > max_L) L_body.y() = max_L;
    if (L_body.y() < -max_L) L_body.y() = -max_L;
    if (L_body.z() > max_L) L_body.z() = max_L;
    if (L_body.z() < -max_L) L_body.z() = -max_L;
    
    omega = Vector3d(L_body.x() / I.x(), L_body.y() / I.y(), L_body.z() / I.z());
    
    double current_energy = 0.5 * (I.x() * omega.x() * omega.x() +
                                   I.y() * omega.y() * omega.y() +
                                   I.z() * omega.z() * omega.z());
    
    if (current_energy > 1e-10 && initial_energy > 1e-10) {
        double energy_ratio = std::sqrt(initial_energy / current_energy);
        double correction = 1.0 + 0.1 * (energy_ratio - 1.0);
        omega *= correction;
        L_body = Vector3d(I.x() * omega.x(), I.y() * omega.y(), I.z() * omega.z());
    }
    
    Eigen::Quaterniond q = quat;
    Eigen::Quaterniond omega_quat(0.0, omega.x(), omega.y(), omega.z());
    Eigen::Quaterniond dq = q * omega_quat;
    quat.w() += 0.5 * dq.w() * dt;
    quat.x() += 0.5 * dq.x() * dt;
    quat.y() += 0.5 * dq.y() * dt;
    quat.z() += 0.5 * dq.z() * dt;
    quat.normalize();
    
    Matrix3d R = quat.toRotationMatrix();
    Vector3d tip_top(2.0, 1.2, 0.0);
    Vector3d tip_bottom(2.0, -1.2, 0.0);
    
    trail_top.push_back(R * tip_top);
    trail_bottom.push_back(R * tip_bottom);
    
    while (trail_top.size() > (size_t)max_trail) {
        trail_top.erase(trail_top.begin());
        trail_bottom.erase(trail_bottom.begin());
    }
}

double RigidBody::getEnergy() const {
    Vector3d omega = getAngularVelocity();
    return 0.5 * (I.x() * omega.x() * omega.x() +
                  I.y() * omega.y() * omega.y() +
                  I.z() * omega.z() * omega.z());
}

double RigidBody::getAngularMomentumMagnitude() const {
    return L_body.norm();
}

Vector3d RigidBody::getAngularVelocity() const {
    return Vector3d(L_body.x() / I.x(), L_body.y() / I.y(), L_body.z() / I.z());
}

void RigidBody::createRaquete() {
    vertices.clear();
    faces.clear();
    
    // Cabo (8 vertices)
    vertices.push_back(glm::vec3(-0.1f, -2.0f, -0.1f));
    vertices.push_back(glm::vec3(0.1f, -2.0f, -0.1f));
    vertices.push_back(glm::vec3(0.1f, -1.0f, -0.1f));
    vertices.push_back(glm::vec3(-0.1f, -1.0f, -0.1f));
    vertices.push_back(glm::vec3(-0.1f, -2.0f, 0.1f));
    vertices.push_back(glm::vec3(0.1f, -2.0f, 0.1f));
    vertices.push_back(glm::vec3(0.1f, -1.0f, 0.1f));
    vertices.push_back(glm::vec3(-0.1f, -1.0f, 0.1f));
    
    // Cabeca (16 lados = 32 vertices frente + 32 tras)
    int n = 16;
    for (int i = 0; i < n; i++) {
        double a = 2.0 * 3.14159265358979323846 * i / n;
        float x = 1.1f * cos(a);
        float y = 0.2f + 0.8f * sin(a);
        vertices.push_back(glm::vec3(x, y, -0.05f));
        vertices.push_back(glm::vec3(x * 0.85f, y * 0.85f, -0.05f));
    }
    for (int i = 0; i < n; i++) {
        double a = 2.0 * 3.14159265358979323846 * i / n;
        float x = 1.1f * cos(a);
        float y = 0.2f + 0.8f * sin(a);
        vertices.push_back(glm::vec3(x, y, 0.05f));
        vertices.push_back(glm::vec3(x * 0.85f, y * 0.85f, 0.05f));
    }
    
    // Faces do cabo
    faces.push_back({{0, 1, 2, 3}});
    faces.push_back({{4, 5, 6, 7}});
    faces.push_back({{0, 1, 5, 4}});
    faces.push_back({{2, 3, 7, 6}});
    faces.push_back({{0, 3, 7, 4}});
    faces.push_back({{1, 2, 6, 5}});
    
    // Faces da cabeca
    int oof = 8, oif = 8 + n, oob = 8 + 2*n, oib = 8 + 3*n;
    for (int i = 0; i < n; i++) {
        int ni = (i + 1) % n;
        faces.push_back({{oof+i, oof+ni, oif+ni, oif+i}});
        faces.push_back({{oob+i, oob+ni, oib+ni, oib+i}});
        faces.push_back({{oof+i, oof+ni, oob+ni, oob+i}});
    }
}

void RigidBody::createBola() {
    vertices.clear();
    faces.clear();
    
    int stacks = 16, slices = 16;
    float r = 1.2f;
    
    for (int i = 0; i <= stacks; i++) {
        double phi = 3.14159265358979323846 * i / stacks;
        for (int j = 0; j <= slices; j++) {
            double theta = 2.0 * 3.14159265358979323846 * j / slices;
            float x = r * sin(phi) * cos(theta);
            float y = r * cos(phi);
            float z = r * sin(phi) * sin(theta);
            vertices.push_back(glm::vec3(x, y, z));
        }
    }
    
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int f = i * (slices + 1) + j;
            int s = f + slices + 1;
            faces.push_back({{f, s, f + 1}});
            faces.push_back({{s, s + 1, f + 1}});
        }
    }
}
