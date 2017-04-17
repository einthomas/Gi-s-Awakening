#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Shader.h"

class ParticleSystem {
public:
    static void init();
    static void makeParticle(const glm::vec3 &position, const glm::vec3 &speed, const glm::vec3 &normal, const glm::vec4 &color);
    static void draw(float delta, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    static void update(float delta);

private:
    struct Particle {
        glm::vec3 originPosition, position, speed;
        glm::vec3 normal;
        float amplitude, frequency;
        glm::vec4 color;
    };

    static const int MAX_PARTICLES = 1000;
    static int particleCount;
    static int bufferStart;
    static int bufferEnd;
    static Shader shader;
    static GLuint VAO;
    static GLuint VBO;
    static GLuint positionVBO;
    static GLuint colorVBO;
    static std::vector<Particle> particles;
    static GLfloat particlePositions[];
    static GLfloat particleColors[];
};
