#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/rigid_body.hpp"
#include <iostream>
#include <algorithm>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

inline double clamp_val(double v, double lo, double hi) {
    return std::max(lo, std::min(hi, v));
}

RigidBody::RigidBody(BodyShape shape) 
    : shape(shape), max_trail(200), position(0.0, 0.0, 0.0), quat(1.0, 0.0, 0.0, 0.0) {
    
    if (shape == BodyShape::BORBOLETA) {
        I = Vector3d(1.0, 2.0, 4.0); color = glm::vec3(0.95f, 0.75f, 0.15f);
        draw_method = "faces"; angular_velocity = Vector3d(0.3, 5.0, 0.3); createBorboleta();
    } else if (shape == BodyShape::RAQUETE) {
        I = Vector3d(0.8, 2.2, 4.0); color = glm::vec3(0.2f, 0.6f, 1.0f);
        draw_method = "faces"; angular_velocity = Vector3d(0.3, 5.0, 0.3); createRaquete();
    } else {
        I = Vector3d(1.0, 1.0, 1.0); color = glm::vec3(1.0f, 0.3f, 0.3f);
        draw_method = "triangles"; angular_velocity = Vector3d(0.3, 5.0, 0.3); createBola();
    }
    
    L_body = Vector3d(I.x()*angular_velocity.x(), I.y()*angular_velocity.y(), I.z()*angular_velocity.z());
    initial_energy = 0.5 * (L_body.x()*angular_velocity.x() + L_body.y()*angular_velocity.y() + L_body.z()*angular_velocity.z());
    L_magnitude = L_body.norm();
    std::cout << "Body created. E=" << initial_energy << std::endl;
}

void RigidBody::createBorboleta() {
    vertices.clear(); faces.clear();
    for (int i = 0; i < 8; i++) {
        double ang = 2.0 * M_PI * i / 8.0;
        vertices.push_back(glm::vec3(0.25f*cos(ang), -0.5f, 0.25f*sin(ang)));
        vertices.push_back(glm::vec3(0.25f*cos(ang), 0.5f, 0.25f*sin(ang)));
    }
    vertices.push_back(glm::vec3(0.25f,0.35f,0)); vertices.push_back(glm::vec3(2.0f,1.2f,0));
    vertices.push_back(glm::vec3(1.6f,0,0)); vertices.push_back(glm::vec3(0.25f,-0.35f,0));
    vertices.push_back(glm::vec3(-0.25f,0.35f,0)); vertices.push_back(glm::vec3(-2.0f,1.2f,0));
    vertices.push_back(glm::vec3(-1.6f,0,0)); vertices.push_back(glm::vec3(-0.25f,-0.35f,0));
    for (int i=16; i<24; i++) vertices.push_back(glm::vec3(vertices[i].x, vertices[i].y, 0.15f));
    for (int i=0; i<8; i++) faces.push_back({{2*i, (2*i+2)%16, (2*i+3)%16, 2*i+1}});
    faces.push_back({{16,17,18,19}}); faces.push_back({{24,25,26,27}});
    faces.push_back({{16,17,25,24}}); faces.push_back({{19,18,26,27}});
    faces.push_back({{17,18,26,25}}); faces.push_back({{16,19,27,24}});
    faces.push_back({{20,21,22,23}}); faces.push_back({{28,29,30,31}});
    faces.push_back({{20,21,29,28}}); faces.push_back({{23,22,30,31}});
    faces.push_back({{21,22,30,29}}); faces.push_back({{20,23,31,28}});
}

void RigidBody::createRaquete() {
    vertices.clear(); faces.clear();
    float p=0.1f;
    vertices.push_back(glm::vec3(-p,-2,-p)); vertices.push_back(glm::vec3(p,-2,-p));
    vertices.push_back(glm::vec3(p,-1,-p)); vertices.push_back(glm::vec3(-p,-1,-p));
    vertices.push_back(glm::vec3(-p,-2,p)); vertices.push_back(glm::vec3(p,-2,p));
    vertices.push_back(glm::vec3(p,-1,p)); vertices.push_back(glm::vec3(-p,-1,p));
    int n=16;
    for(int i=0;i<n;i++){
        double a=2.0*M_PI*i/n; float x=1.1f*cos(a),y=0.2f+0.8f*sin(a);
        vertices.push_back(glm::vec3(x,y,-0.05f)); vertices.push_back(glm::vec3(x*0.85f,y*0.85f,-0.05f));
    }
    for(int i=0;i<n;i++){
        double a=2.0*M_PI*i/n; float x=1.1f*cos(a),y=0.2f+0.8f*sin(a);
        vertices.push_back(glm::vec3(x,y,0.05f)); vertices.push_back(glm::vec3(x*0.85f,y*0.85f,0.05f));
    }
    faces.push_back({{0,1,2,3}}); faces.push_back({{4,5,6,7}});
    faces.push_back({{0,1,5,4}}); faces.push_back({{2,3,7,6}});
    faces.push_back({{0,3,7,4}}); faces.push_back({{1,2,6,5}});
    int oof=8,oif=8+n,oob=8+2*n,oib=8+3*n;
    for(int i=0;i<n;i++){
        int ni=(i+1)%n;
        faces.push_back({{oof+i,oof+ni,oif+ni,oif+i}});
        faces.push_back({{oob+i,oob+ni,oib+ni,oib+i}});
        faces.push_back({{oof+i,oof+ni,oob+ni,oob+i}});
    }
}

void RigidBody::createBola() {
    vertices.clear(); faces.clear();
    int stacks=16,slices=16; float r=1.2f;
    for(int i=0;i<=stacks;i++){
        double phi=M_PI*i/stacks;
        for(int j=0;j<=slices;j++){
            double theta=2.0*M_PI*j/slices;
            vertices.push_back(glm::vec3(r*sin(phi)*cos(theta),r*cos(phi),r*sin(phi)*sin(theta)));
        }
    }
    for(int i=0;i<stacks;i++)
        for(int j=0;j<slices;j++){
            int f=i*(slices+1)+j,s=f+slices+1;
            faces.push_back({{f,s,f+1}}); faces.push_back({{s,s+1,f+1}});
        }
}

void RigidBody::update(double dt, bool gravity_enabled, double gravity_accel) {
    dt = std::min(dt, 0.016);
    
    Vector3d I_inv(1.0/I.x(), 1.0/I.y(), 1.0/I.z());
    Vector3d omega(L_body.x()*I_inv.x(), L_body.y()*I_inv.y(), L_body.z()*I_inv.z());
    
    // dL/dt = L x omega (Euler equations)
    double Lx = L_body.x(), Ly = L_body.y(), Lz = L_body.z();
    double ox = omega.x(), oy = omega.y(), oz = omega.z();
    
    L_body.x() += dt * (Ly*oz - Lz*oy);
    L_body.y() += dt * (Lz*ox - Lx*oz);
    L_body.z() += dt * (Lx*oy - Ly*ox);
    
    L_body.x() = clamp_val(L_body.x(), -100.0, 100.0);
    L_body.y() = clamp_val(L_body.y(), -100.0, 100.0);
    L_body.z() = clamp_val(L_body.z(), -100.0, 100.0);
    
    omega = Vector3d(L_body.x()*I_inv.x(), L_body.y()*I_inv.y(), L_body.z()*I_inv.z());
    double E = 0.5 * (L_body.x()*omega.x() + L_body.y()*omega.y() + L_body.z()*omega.z());
    
    if (E > 0 && std::abs(E - initial_energy) > 0.001 * initial_energy) {
        double scale = std::sqrt(initial_energy / E);
        L_body.x() *= scale; L_body.y() *= scale; L_body.z() *= scale;
    }
    
    omega = Vector3d(L_body.x()*I_inv.x(), L_body.y()*I_inv.y(), L_body.z()*I_inv.z());
    double wx = omega.x(), wy = omega.y(), wz = omega.z();
    double qw = quat.w(), qx = quat.x(), qy = quat.y(), qz = quat.z();
    
    quat.w() += 0.5 * (-qx*wx - qy*wy - qz*wz) * dt;
    quat.x() += 0.5 * ( qw*wx + qy*wz - qz*wy) * dt;
    quat.y() += 0.5 * ( qw*wy - qx*wz + qz*wx) * dt;
    quat.z() += 0.5 * ( qw*wz + qx*wy - qy*wx) * dt;
    quat.normalize();
    
    Matrix3d R = quat.toRotationMatrix();
    trail_top.push_back(R * Vector3d(2.0, 1.2, 0.0));
    trail_bottom.push_back(R * Vector3d(2.0, -1.2, 0.0));
    while (trail_top.size() > (size_t)max_trail) {
        trail_top.erase(trail_top.begin());
        trail_bottom.erase(trail_bottom.begin());
    }
}

double RigidBody::getEnergy() const {
    Vector3d omega = getAngularVelocity();
    return 0.5 * (L_body.x()*omega.x() + L_body.y()*omega.y() + L_body.z()*omega.z());
}

double RigidBody::getAngularMomentumMagnitude() const { return L_body.norm(); }

Vector3d RigidBody::getAngularVelocity() const {
    return Vector3d(L_body.x()/I.x(), L_body.y()/I.y(), L_body.z()/I.z());
}
