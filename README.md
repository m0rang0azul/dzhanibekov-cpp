# Dzhanibekov Effect Simulation - C++

-This isn't the final version of the code, the physics of the problem is still being worked on 🍓

-A physically accurate C++ implementation of the Dzhanibekov effect (tennis racket theorem) using modern OpenGL for real-time visualization.

## Overview

The Dzhanibekov effect is a striking phenomenon in rotational dynamics where an asymmetric object (like a tennis racket) flips unpredictably when spun. This simulation demonstrates the conservation of angular momentum (L) and kinetic energy (E) in rigid body dynamics.

### Physics Features

- **Symplectic Integration**: Uses splitting methods for energy-preserving numerical integration
- **Conservation Laws**:
  - Angular momentum **L** is strictly conserved (no external torque)
  - Kinetic energy **E** is strictly conserved
- **Euler Equations**: Implements the full rotational equations of motion for rigid bodies
- **Three Body Shapes**:
  - **Butterfly** (asymmetric wings)
  - **Tennis Racket** (asymmetric mass distribution)
  - **Sphere** (symmetric - no flipping)

### Optional Features

- **Gravity**: Apply external torque for realistic drop behavior
- **Flip Detection**: Automatic counting of complete 180° rotations
- **Visual Trails**: Track the motion of the racket tips through space
- **Interactive Camera**: Mouse-controlled viewing angle

## Building

### Requirements

- CMake 3.10+
- C++17 compiler
- OpenGL 3.3+
- GLFW3
- GLM
- Eigen3

### Ubuntu/Debian

```bash
sudo apt-get install libglfw3-dev libglm-dev libeigen3-dev
```

### macOS (Homebrew)

```bash
brew install glfw3 glm eigen
```

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make -j4
./dzhanibekov
```

## Controls

| Key | Action |
|-----|--------|
| **1** | Select Butterfly shape |
| **2** | Select Tennis Racket shape |
| **3** | Select Sphere shape |
| **I** | Enable minimal gravity |
| **E** | Enable Earth gravity (9.81 m/s²) |
| **0** | Disable gravity |
| **SPACE** | Pause/Resume |
| **R** | Reset to initial conditions |
| **↑/↓** | Increase/Decrease simulation speed |
| **Mouse Drag** | Rotate camera |
| **ESC** | Exit |

## Physics Explanation

### Rigid Body Dynamics

The simulation solves Euler's rotational equations:

```
dL/dt = L × (I⁻¹·L)
```

Where:
- **L** = Angular momentum vector (conserved)
- **I** = Inertia tensor (diagonal in principal axes)
- **ω** = Angular velocity (I⁻¹·L)

### Why Objects Flip

For asymmetric objects:
1. Different moments of inertia (I₁ < I₂ < I₃) cause unstable rotation about the intermediate axis
2. Small perturbations cause the object to precess
3. The angular momentum vector slowly reorients in space
4. When the object's reference frame aligns opposite to the lab frame → **FLIP**

Symmetric objects (sphere) never flip because I₁ = I₂ = I₃.

### Energy & Momentum Conservation

The code includes verification that:

```cpp
E = 0.5 * L·ω = CONSTANT
|L| = CONSTANT
```

Small energy drift is automatically corrected each frame by rescaling L.

## Implementation Details

### File Structure

```
include/
├── math_utils.hpp      # Quaternion & linear algebra
├── rigid_body.hpp      # RigidBody class
├── simulation.hpp      # Main simulation loop
├── renderer.hpp        # OpenGL rendering
└── geometry.hpp        # Shape generators

src/
├── main.cpp           # Entry point
├── rigid_body.cpp     # Physics solver
├── simulation.cpp     # Simulation control
├── renderer.cpp       # Rendering code
└── geometry.cpp       # Geometry creation
```

### Key Classes

#### Quaternion
Represents 3D rotations with smooth interpolation and integration.

#### RigidBody
Manages:
- Rotational state (quaternion + angular momentum)
- Moment of inertia tensor
- Vertex/face geometry
- Physics updates via Euler equations
- Trail tracking for visualization

#### Renderer
Handles:
- OpenGL context and window management
- 3D scene rendering with lighting
- Trail visualization
- Camera control

## Performance

- **CPU Optimized**: Uses Eigen3 for SIMD-accelerated linear algebra
- **Framerate**: Maintains 60 FPS on modern hardware
- **Precision**: Double-precision floating point for numerical stability

## Comparison with Python Version

| Aspect | Python | C++ |
|--------|--------|-----|
| Speed | ~30 FPS | ~60 FPS |
| Precision | Single (float) | Double (double) |
| Libraries | NumPy, Pygame | Eigen3, GLFW3 |
| Platform | Cross-platform | Cross-platform |
| Math Operations | Vectorized | SIMD-accelerated |

## References

- **Dzhanibekov Effect**: https://en.wikipedia.org/wiki/Tennis_racket_theorem
- **Rigid Body Dynamics**: Goldstein, Classical Mechanics
- **Symplectic Integration**: Hairer et al., Geometric Numerical Integration

## License

MIT License

## Author

C++ port based on Python implementation by Luana M. Souza
