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
    static GLuint skyboxCubeVAO;

    static Object3D makeCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);

    static Object3D makeSkyboxCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

    bool intersects(glm::vec3 position, glm::vec3 scale);

    void solveCollision(glm::vec3 &position, glm::vec3 &velocity, const glm::vec3 &scale, bool &onGround) const;
};

