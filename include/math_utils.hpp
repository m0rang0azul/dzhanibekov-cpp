#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using Vector3d = Eigen::Vector3d;
using Vector4d = Eigen::Vector4d;
using Matrix3d = Eigen::Matrix3d;
using Matrix4d = Eigen::Matrix4d;

class Quaternion {
public:
    double w, x, y, z;

    Quaternion() : w(1.0), x(0.0), y(0.0), z(0.0) {}
    Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}
    Quaternion(const Eigen::Vector4d& v) : w(v(0)), x(v(1)), y(v(2)), z(v(3)) {}

    // Normalize quaternion
    Quaternion normalized() const {
        double norm = std::sqrt(w*w + x*x + y*y + z*z);
        if (norm < 1e-10) return Quaternion(1, 0, 0, 0);
        return Quaternion(w/norm, x/norm, y/norm, z/norm);
    }

    // Convert to rotation matrix
    Matrix3d toRotationMatrix() const {
        double w_c = std::max(-1.0, std::min(1.0, w));
        double x_c = std::max(-1.0, std::min(1.0, x));
        double y_c = std::max(-1.0, std::min(1.0, y));
        double z_c = std::max(-1.0, std::min(1.0, z));

        Matrix3d R;
        R(0,0) = 1 - 2*y_c*y_c - 2*z_c*z_c;
        R(0,1) = 2*x_c*y_c - 2*w_c*z_c;
        R(0,2) = 2*x_c*z_c + 2*w_c*y_c;
        
        R(1,0) = 2*x_c*y_c + 2*w_c*z_c;
        R(1,1) = 1 - 2*x_c*x_c - 2*z_c*z_c;
        R(1,2) = 2*y_c*z_c - 2*w_c*x_c;
        
        R(2,0) = 2*x_c*z_c - 2*w_c*y_c;
        R(2,1) = 2*y_c*z_c + 2*w_c*x_c;
        R(2,2) = 1 - 2*x_c*x_c - 2*y_c*y_c;
        
        return R;
    }

    // Quaternion derivative from angular velocity
    Quaternion derivative(const Vector3d& omega) const {
        double dw = 0.5 * (-x*omega(0) - y*omega(1) - z*omega(2));
        double dx = 0.5 * (w*omega(0) + y*omega(2) - z*omega(1));
        double dy = 0.5 * (w*omega(1) - x*omega(2) + z*omega(0));
        double dz = 0.5 * (w*omega(2) + x*omega(1) - y*omega(0));
        return Quaternion(dw, dx, dy, dz);
    }

    Eigen::Vector4d toVector4d() const {
        return Eigen::Vector4d(w, x, y, z);
    }
};

inline Vector3d crossProduct(const Vector3d& a, const Vector3d& b) {
    return a.cross(b);
}

#endif
