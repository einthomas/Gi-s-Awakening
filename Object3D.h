#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"
#include "Mesh.h"

class Object3D {
public:
    Material *material;

    glm::vec3 position, scale;
    Mesh mesh;
    glm::vec3 size;

    static GLuint cubeVAO;
    static GLuint skyboxCubeVAO;

    static Object3D makeCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);
    static Object3D fromFile(Material *material, const glm::vec3 &position, const glm::vec3 &scale, const char *filename);

    static Object3D makeSkyboxCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
};

