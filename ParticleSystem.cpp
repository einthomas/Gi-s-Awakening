#include "ParticleSystem.h"

int ParticleSystem::particleCount = 0;
int ParticleSystem::bufferStart = 0;
int ParticleSystem::bufferEnd = 0;
int ParticleSystem::currentPerlinFlowField = -1;
Shader ParticleSystem::shader;
GLuint ParticleSystem::VAO = static_cast<GLuint>(-1);
GLuint ParticleSystem::VBO;
GLuint ParticleSystem::positionVBO;
GLuint ParticleSystem::colorVBO;
std::vector<ParticleSystem::Particle> ParticleSystem::particles(MAX_PARTICLES);
GLfloat ParticleSystem::particlePositions[MAX_PARTICLES * 3 + 3];
GLfloat ParticleSystem::particleColors[MAX_PARTICLES * 4 + 4];
std::vector<std::vector<std::vector<glm::vec3>>> ParticleSystem::perlinFlowFields(MAX_PARTICLES / 20);
std::vector<std::vector<std::vector<float>>> ParticleSystem::perlinAngles(PERLIN_FLOW_FIELDS);

static GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
     0.5f,  0.5f, 0.0f
};

void ParticleSystem::init() {
    shader = Shader("shaders/particle.vert", "shaders/particle.frag");

    const float PERLIN_STEP = 0.1f;
    glm::vec3 perlinOffset(0.0f);
    for (int m = 0; m < PERLIN_FLOW_FIELDS; m++) {
        perlinAngles[m] = std::vector<std::vector<float>>(PERLIN_FLOW_FIELD_SIZE);
        perlinOffset.y = 0.0f;
        for (int i = 0; i < PERLIN_FLOW_FIELD_SIZE; i++) {
            perlinAngles[m][i] = std::vector<float>(PERLIN_FLOW_FIELD_SIZE);
            for (int k = 0; k < PERLIN_FLOW_FIELD_SIZE; k++) {
                perlinAngles[m][i][k] = glm::perlin<float>(perlinOffset) * glm::pi<float>() * 8.0f;
                perlinOffset.x += PERLIN_STEP;
            }
            perlinOffset.x = 0.0f;
            perlinOffset.y += PERLIN_STEP;
        }
        perlinOffset.z += 0.1f;
    }

    for (int m = 0; m < perlinFlowFields.size(); m++) {
        for (int i = 0; i < PERLIN_FLOW_FIELD_SIZE; i++) {
            perlinFlowFields[m].push_back(std::vector<glm::vec3>(PERLIN_FLOW_FIELD_SIZE));
        }
    }
}

void ParticleSystem::beginParticleGroup(const glm::vec3 &planeNormal) {
    currentPerlinFlowField++;
    currentPerlinFlowField %= perlinFlowFields.size();

    float x = 1.0f * planeNormal.x;
    float y = 1.0f * planeNormal.y;
    float z = (-x - y) / planeNormal.z;
    const glm::vec3 vectorWithinPlane(x, y, z);
    
    for (int i = 0; i < PERLIN_FLOW_FIELD_SIZE; i++) {
        for (int k = 0; k < PERLIN_FLOW_FIELD_SIZE; k++) {
            float perlinAngle = perlinAngles[currentPerlinFlowField % 2][i][k];
            glm::vec3 particleVector = vectorWithinPlane;
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), perlinAngle, planeNormal);
            particleVector = rotationMatrix * glm::vec4(particleVector, 1.0f);

            particleVector = glm::normalize(particleVector);
            perlinFlowFields[currentPerlinFlowField][i][k] = particleVector;
        }
    }
}

void ParticleSystem::makeParticle(const glm::vec3 &position) {
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
    particle.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    particle.perlinFlowField = currentPerlinFlowField;
    particle.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    particle.perlinFlowField = currentPerlinFlowField;
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
            int xIndex = floor(fmodf(particles[i].position.x * 20.0f, PERLIN_FLOW_FIELD_SIZE));
            xIndex = xIndex < 0 ? PERLIN_FLOW_FIELD_SIZE + xIndex : xIndex;

            int yIndex = floor(fmodf(particles[i].position.y * 20.0f, PERLIN_FLOW_FIELD_SIZE));
            yIndex = yIndex < 0 ? PERLIN_FLOW_FIELD_SIZE + yIndex : yIndex;

            particles[i].velocity += perlinFlowFields[currentPerlinFlowField][xIndex][yIndex] * 0.1f;
            particles[i].velocity = MathUtil::limit(particles[i].velocity, 1.0f);
            particles[i].position += particles[i].velocity * delta;

            particlePositions[3 * k] = particles[i].position.x;
            particlePositions[3 * k + 1] = particles[i].position.y;
            particlePositions[3 * k + 2] = particles[i].position.z;

            particleColors[4 * k] = particles[i].color.x;
            particleColors[4 * k + 1] = particles[i].color.y;
            particleColors[4 * k + 2] = particles[i].color.z;
            particleColors[4 * k + 3] = particles[i].color.a;

            particles[i].color.a -= delta * 1.5f;
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
