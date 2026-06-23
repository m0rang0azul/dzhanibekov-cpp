#include "geometry.hpp"
#include <cmath>

void GeometryGenerator::generateBorboleta(
    std::vector<glm::vec3>& vertices,
    std::vector<Face>& faces) {
    
    vertices.clear();
    faces.clear();
    
    // Body (circular cross-section)
    for (int i = 0; i < 8; ++i) {
        float ang = 2.0f * M_PI * i / 8.0f;
        float x = 0.25f * std::cos(ang);
        float z = 0.25f * std::sin(ang);
        vertices.push_back(glm::vec3(x, -0.5f, z));
        vertices.push_back(glm::vec3(x, 0.5f, z));
    }
    
    // Wings
    vertices.push_back(glm::vec3(0.25f, 0.35f, 0));
    vertices.push_back(glm::vec3(2.0f, 1.2f, 0));
    vertices.push_back(glm::vec3(1.6f, 0, 0));
    vertices.push_back(glm::vec3(0.25f, -0.35f, 0));
    
    vertices.push_back(glm::vec3(-0.25f, 0.35f, 0));
    vertices.push_back(glm::vec3(-2.0f, 1.2f, 0));
    vertices.push_back(glm::vec3(-1.6f, 0, 0));
    vertices.push_back(glm::vec3(-0.25f, -0.35f, 0));
    
    // Wings with depth
    for (int i = 16; i < 24; ++i) {
        auto v = vertices[i];
        vertices.push_back(glm::vec3(v.x, v.y, 0.15f));
    }
    
    // Body faces
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

void GeometryGenerator::generateRaquete(
    std::vector<glm::vec3>& vertices,
    std::vector<Face>& faces) {
    
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
    for (int i = 0; i < n; ++i) {
        float a = 2.0f * M_PI * i / n;
        float x = 1.1f * std::cos(a);
        float y = 0.2f + 0.8f * std::sin(a);
        vertices.push_back(glm::vec3(x, y, -0.05f));
        vertices.push_back(glm::vec3(x*0.85f, y*0.85f, -0.05f));
    }
    
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
    
    // Racket faces
    int oof = 8, oif = 8 + n, oob = 8 + 2*n, oib = 8 + 3*n;
    for (int i = 0; i < n; ++i) {
        int ni = (i+1) % n;
        faces.push_back(Face{{oof+i, oof+ni, oif+ni, oif+i}});
        faces.push_back(Face{{oob+i, oob+ni, oib+ni, oib+i}});
        faces.push_back(Face{{oof+i, oof+ni, oob+ni, oob+i}});
    }
}

void GeometryGenerator::generateBola(
    std::vector<glm::vec3>& vertices,
    std::vector<Face>& faces) {
    
    vertices.clear();
    faces.clear();
    
    int stacks = 16, slices = 16;
    float r = 1.2f;
    
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
    
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int f = i * (slices + 1) + j;
            int s = f + slices + 1;
            faces.push_back(Face{{f, s, f+1}});
            faces.push_back(Face{{s, s+1, f+1}});
        }
    }
}
