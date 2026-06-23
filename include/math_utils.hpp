#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using Vector3d = Eigen::Vector3d;
using Matrix3d = Eigen::Matrix3d;
using Quaternion = Eigen::Quaterniond;

// Use outro nome para evitar conflito
inline double limit_value(double value, double min_val, double max_val) {
    if (value > max_val) return max_val;
    if (value < min_val) return min_val;
    return value;
}

#endif
