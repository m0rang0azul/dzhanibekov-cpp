#include <windows.h>  // PRECISA vir antes do GL
#include <gl/GL.h>
#include <cmath>
#include "../include/rigid_body.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Implementação manual do gluPerspective
static void myPerspective(double fovy, double aspect, double zNear, double zFar) {
    double f = 1.0 / tan(fovy * M_PI / 360.0);
    double mat[16] = {
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zFar + zNear) / (zNear - zFar), -1,
        0, 0, (2 * zFar * zNear) / (zNear - zFar), 0
    };
    glMultMatrixd(mat);
}

// Implementação manual do gluLookAt
static void myLookAt(double eyex, double eyey, double eyez,
                     double centerx, double centery, double centerz,
                     double upx, double upy, double upz) {
    double f[3] = {centerx - eyex, centery - eyey, centerz - eyez};
    double fLen = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= fLen; f[1] /= fLen; f[2] /= fLen;
    
    double up[3] = {upx, upy, upz};
    double s[3] = {f[1]*up[2] - f[2]*up[1], f[2]*up[0] - f[0]*up[2], f[0]*up[1] - f[1]*up[0]};
    double sLen = sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
    s[0] /= sLen; s[1] /= sLen; s[2] /= sLen;
    
    double u[3] = {s[1]*f[2] - s[2]*f[1], s[2]*f[0] - s[0]*f[2], s[0]*f[1] - s[1]*f[0]};
    
    double mat[16] = {
        s[0], u[0], -f[0], 0,
        s[1], u[1], -f[1], 0,
        s[2], u[2], -f[2], 0,
        0, 0, 0, 1
    };
    glMultMatrixd(mat);
    glTranslated(-eyex, -eyey, -eyez);
}

void renderGrid() {
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.2f, 0.3f);
    glBegin(GL_LINES);
    for (int i = -5; i <= 5; i++) {
        glVertex3f((float)i, -2.5f, -5.0f);
        glVertex3f((float)i, -2.5f, 5.0f);
        glVertex3f(-5.0f, -2.5f, (float)i);
        glVertex3f(5.0f, -2.5f, (float)i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void renderAxes() {
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-3.0f, 0.0f, 0.0f); glVertex3f(3.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f, -3.0f, 0.0f); glVertex3f(0.0f, 3.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -3.0f); glVertex3f(0.0f, 0.0f, 3.0f);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void setupCamera(double cam_dist, double cam_angle_x, double cam_angle_y, int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    myPerspective(45.0, (double)width / height, 0.1, 60.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    double cx = cam_dist * sin(cam_angle_y * M_PI / 180.0) * cos(cam_angle_x * M_PI / 180.0);
    double cy = cam_dist * sin(cam_angle_x * M_PI / 180.0);
    double cz = cam_dist * cos(cam_angle_y * M_PI / 180.0) * cos(cam_angle_x * M_PI / 180.0);
    
    myLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void renderRigidBody(const RigidBody& body) {
    glPushMatrix();
    
    Vector3d pos = body.getPosition();
    glTranslatef((float)pos.x(), (float)pos.y(), (float)pos.z());
    
    Matrix3d R = body.getRotationMatrix();
    float mat[16] = {
        (float)R(0,0), (float)R(1,0), (float)R(2,0), 0.0f,
        (float)R(0,1), (float)R(1,1), (float)R(2,1), 0.0f,
        (float)R(0,2), (float)R(1,2), (float)R(2,2), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(mat);
    
    // Rastros
    glDisable(GL_LIGHTING);
    const auto& trail_top = body.getTrailTop();
    const auto& trail_bottom = body.getTrailBottom();
    
    if (trail_top.size() > 1) {
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < trail_top.size(); i++) {
            float alpha = (float)i / trail_top.size();
            glColor4f(1.0f, 1.0f, 0.0f, alpha);
            glVertex3d(trail_top[i].x(), trail_top[i].y(), trail_top[i].z());
        }
        glEnd();
        
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < trail_bottom.size(); i++) {
            float alpha = (float)i / trail_bottom.size();
            glColor4f(1.0f, 0.5f, 0.0f, alpha);
            glVertex3d(trail_bottom[i].x(), trail_bottom[i].y(), trail_bottom[i].z());
        }
        glEnd();
        glLineWidth(1.0f);
    }
    glEnable(GL_LIGHTING);
    
    // Corpo
    glm::vec3 color = body.getColor();
    const auto& vertices = body.getVertices();
    const auto& faces = body.getFaces();
    const std::string& method = body.getDrawMethod();
    
    if (method == "triangles") {
        glBegin(GL_TRIANGLES);
        for (const auto& face : faces) {
            float shade = 0.6f + 0.4f * (face.indices[0] % 10) / 10.0f;
            glColor3f(color.r * shade, color.g * shade, color.b * shade);
            for (int idx : face.indices) {
                glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
            }
        }
        glEnd();
    } else {
        glBegin(GL_QUADS);
        for (size_t i = 0; i < faces.size(); i++) {
            float shade = 0.6f + (i % 8) * 0.05f;
            glColor3f(color.r * shade, color.g * shade, color.b * shade);
            for (int idx : faces[i].indices) {
                glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
            }
        }
        glEnd();
    }
    
    // Wireframe
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    for (const auto& face : faces) {
        glBegin(GL_LINE_LOOP);
        for (int idx : face.indices) {
            glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}
