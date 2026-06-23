#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm/glm.hpp>
#include <vector>
#include "rigid_body.hpp"

class GeometryGenerator {
public:
    static void generateBorboleta(
        std::vector<glm::vec3>& vertices,
        std::vector<Face>& faces
    );
    
    static void generateRaquete(
        std::vector<glm::vec3>& vertices,
        std::vector<Face>& faces
    );
    
    static void generateBola(
        std::vector<glm::vec3>& vertices,
        std::vector<Face>& faces
    );
};

#endif
