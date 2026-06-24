#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

#include "math_utils.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>

enum class BodyShape {
    BORBOLETA,
    RAQUETE,
    BOLA
};

struct Face {
    std::vector<int> indices;
};

class RigidBody {
private:
    BodyShape shape;
    std::vector<glm::vec3> vertices;
    std::vector<Face> faces;
    
    Vector3d I;
    Vector3d L_body;
    Vector3d angular_velocity;
    double initial_energy;
    double L_magnitude;
    
    Quaternion quat;
    Vector3d position;
    
    std::vector<Vector3d> trail_top;
    std::vector<Vector3d> trail_bottom;
    int max_trail;
    
    glm::vec3 color;
    std::string draw_method;
    
    void createBorboleta();
    void createRaquete();
    void createBola();
    
public:
    RigidBody(BodyShape shape = BodyShape::BORBOLETA);
    
    void update(double dt, bool gravity_enabled = false, double gravity_accel = 9.81);
    
    const std::vector<glm::vec3>& getVertices() const { return vertices; }
    const std::vector<Face>& getFaces() const { return faces; }
    const glm::vec3& getColor() const { return color; }
    const std::string& getDrawMethod() const { return draw_method; }
    const Quaternion& getQuaternion() const { return quat; }
    const Vector3d& getPosition() const { return position; }
    const std::vector<Vector3d>& getTrailTop() const { return trail_top; }
    const std::vector<Vector3d>& getTrailBottom() const { return trail_bottom; }
    Matrix3d getRotationMatrix() const { return quat.toRotationMatrix(); }
    
    double getEnergy() const;
    double getAngularMomentumMagnitude() const;
    Vector3d getAngularVelocity() const;
    Vector3d getLBody() const { return L_body; }
    BodyShape getShape() const { return shape; }
};

#endif

