#include "ParticleSystem.h"
#include <iostream>

int ParticleSystem::particleCount = 0;
int ParticleSystem::bufferStart = 0;
int ParticleSystem::bufferEnd = 0;
Shader ParticleSystem::shader;
GLuint ParticleSystem::VAO = static_cast<GLuint>(-1);
GLuint ParticleSystem::VBO;
GLuint ParticleSystem::positionVBO;
GLuint ParticleSystem::colorVBO;
std::vector<ParticleSystem::Particle> ParticleSystem::particles(MAX_PARTICLES);
GLfloat ParticleSystem::particlePositions[MAX_PARTICLES * 3 + 3];
GLfloat ParticleSystem::particleColors[MAX_PARTICLES * 4 + 4];

static GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
     0.5f,  0.5f, 0.0f
};

void ParticleSystem::init() {
    shader = Shader("shaders/particle.vert", "shaders/particle.frag");
}

void ParticleSystem::makeParticle(const glm::vec3 &position, const glm::vec3 &speed, const glm::vec3 &normal, const glm::vec4&color) {
    if (VAO == static_cast<GLuint>(-1)) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &positionVBO);
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLvoid*>(0));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, static_cast<GLvoid*>(0));
        glEnableVertexAttribArray(2);

        glVertexAttribDivisor(0, 0);    // use same vertices for all instances
        glVertexAttribDivisor(1, 1);    // advance position VBO once per instance
        glVertexAttribDivisor(2, 1);    // advance color VBO once per instance

        glBindVertexArray(0);
    }

    Particle particle;
    particle.position = position;
    particle.originPosition = position;
    particle.normal = normal;
    particle.speed = speed * (((rand() % 20) / 10.0f) + 0.5f);
    particle.color = color;
    particle.life = 4;
    particle.amplitude = 5.0f + (rand() % 20) / 5.0f;
    particle.frequency = (rand() % 90) / 1000.0f;
    particles[bufferEnd] = particle;
    if (particleCount < MAX_PARTICLES) {
        particleCount++;
    }
    bufferEnd++;
    if (bufferEnd == MAX_PARTICLES) {
        bufferEnd = 0;
    }
}

void ParticleSystem::draw(float delta, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    if (particleCount == 0) {
        return;
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * particleCount, NULL, GL_STREAM_DRAW);   // reallocate storage "buffer orphaning"
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * particleCount, particlePositions);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * particleCount, NULL, GL_STREAM_DRAW);   // reallocate storage "buffer orphaning"
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * particleCount, particleColors);

    shader.use();
    shader.setMatrix4("view", viewMatrix);
    shader.setMatrix4("projection", projectionMatrix);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);

    glBindVertexArray(0);
}

void ParticleSystem::update(float delta) {
    for (int i = bufferStart, k = 0; k < particleCount; i++, k++) {
        if (i == MAX_PARTICLES) {
            i = 0;
            if (i == bufferEnd) {
                break;
            }
        }
        if (particles[i].color.a > 0.0f) {
            float dist = glm::length(particles[i].position - particles[i].originPosition);
            particles[i].position +=
                particles[i].speed * delta +
                particles[i].normal * sinf(dist * particles[i].amplitude) * particles[i].frequency;

            particlePositions[3 * k] = particles[i].position.x;
            particlePositions[3 * k + 1] = particles[i].position.y;
            particlePositions[3 * k + 2] = particles[i].position.z;

            particleColors[4 * k] = particles[i].color.x;
            particleColors[4 * k + 1] = particles[i].color.y;
            particleColors[4 * k + 2] = particles[i].color.z;
            particleColors[4 * k + 3] = particles[i].color.a;

            particles[i].color.a -= 0.01f;
            if (particles[i].color.a <= 0.0f) {
                particleCount--;
                bufferStart++;
                if (bufferStart == MAX_PARTICLES) {
                    bufferStart = 0;
                }
            }
        }
    }
}
