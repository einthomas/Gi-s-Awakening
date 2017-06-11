#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

class Object3D {
public:
    Material *material;
    glm::vec3 position, scale, size;
    Mesh mesh;

    static GLuint cubeVAO;
    static GLuint skyboxCubeVAO;

    Object3D(Material *material, glm::vec3 position, glm::vec3 scale, Mesh mesh);
    Object3D(Material *material, glm::vec3 position, glm::vec3 scale, glm::vec3 size, Mesh mesh);

    static Object3D makeCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);

    static Object3D makeSkyboxCube(Material *material, const glm::vec3 &position, const glm::vec3 &scale);

    void draw(const Shader &shader);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void draw(RenderInfo renderInfo, ShadowInfo shadowInfo);

private:
    glm::mat4 calculateModelMatrix();
};

