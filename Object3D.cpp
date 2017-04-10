#include "Object3D.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

GLuint Object3D::cubeVAO = static_cast<GLuint>(-1);

static GLfloat vertices[] = {
    // Positions          // Texture Coords // Normals
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,        0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,        0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,        0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,        0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,        0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,        0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,        0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,        0.0f,  1.0f,  0.0f,
};

Object3D Object3D::makeCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale) {
    if (cubeVAO == static_cast<GLuint>(-1)) {
        // create and bind VAO
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);

        // create and bind VBO
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // copy vertices array to VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), static_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        // unbind VAO
        glBindVertexArray(0);
    }

    return { material, position, scale, { cubeVAO, sizeof(vertices) / 8 / 4 } };
}

Object3D Object3D::fromFile(Material *material, const glm::vec3 &position, const glm::vec3 &scale, const char *filename) {
    return { material, position, scale, Mesh::fromFile(filename) };
}

void Object3D::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    material->bind(viewMatrix, projectionMatrix, modelMatrix);

    mesh.draw();
}

bool Object3D::intersects(glm::vec3 position, glm::vec3 scale) {
    if (
        position.x + scale.x / 2.0f > this->position.x - this->scale.x / 2.0f &&
        position.x - scale.x / 2.0f < this->position.x + this->scale.x / 2.0f &&
        position.y + scale.y / 2.0f > this->position.y - this->scale.y / 2.0f &&
        position.y - scale.y / 2.0f < this->position.y + this->scale.y / 2.0f &&
        position.z + scale.z / 2.0f > this->position.z - this->scale.z / 2.0f &&
        position.z - scale.z / 2.0f < this->position.z + this->scale.z / 2.0f
    ) {
        return true;
    }

    return false;
}
