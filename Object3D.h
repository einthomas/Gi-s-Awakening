#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"

class Object3D {
public:
    Material *material;

    glm::vec3 position, scale;
    GLuint VAO, elementCount;

    static GLuint cubeVAO;

    static Object3D makeCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
};

