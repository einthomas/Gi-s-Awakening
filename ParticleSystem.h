#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>

#include "Shader.h"
#include "MathUtil.h"

class ParticleSystem {
public:
    static void init();
    static void beginParticleGroup(const glm::vec3 &planeNormal);
    static void makeParticle(const glm::vec3 &position);
    static void draw(float delta, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    static void update(float delta);

private:
    struct Particle {
        glm::vec3 originPosition, position;
        glm::vec4 color;
        int perlinFlowField;
        glm::vec3 velocity;
    };

    static const int MAX_PARTICLES = 1000;
    static const int PERLIN_FLOW_FIELD_SIZE = 20;
    static const int PERLIN_FLOW_FIELDS = 3;
    static int particleCount;
    static int bufferStart;
    static int bufferEnd;
    static int currentPerlinFlowField;
    static Shader shader;
    static GLuint VAO;
    static GLuint VBO;
    static GLuint positionVBO;
    static GLuint colorVBO;
    static std::vector<Particle> particles;
    static GLfloat particlePositions[];
    static GLfloat particleColors[];
    static std::vector<std::vector<std::vector<glm::vec3>>> perlinFlowFields;
    static std::vector<std::vector<std::vector<float>>> perlinAngles;
};
