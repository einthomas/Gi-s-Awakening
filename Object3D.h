#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

class Object3D {
public:
    static int objectDrawCount;
    static bool frustumCullingEnabled;

    Material *material;
    glm::vec3 position, scale, size;
    Mesh mesh;

    GLfloat lightMapScale;
    glm::vec2 lightMapPosition;

    static GLuint cubeVAO;
    static GLuint skyboxCubeVAO;

    Object3D(
        Material *material, glm::vec3 position, glm::vec3 scale, Mesh mesh
    );
    Object3D(
        Material *material, const glm::vec3 &position, const glm::vec3 &scale,
        const glm::vec3 &size, const Mesh &mesh, GLfloat lightMapScale = 0,
        glm::vec2 lightMapPosition = glm::vec2(0)
    );
    virtual ~Object3D() = default;

    static Object3D makeCube(
        Material *material, const glm::vec3 &position, const glm::vec3 &scale
    );

    static Object3D makeSkyboxCube(
        Material *material, const glm::vec3 &position, const glm::vec3 &scale
    );

    void draw(const Shader &shader);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void draw(RenderInfo renderInfo, ShadowInfo shadowInfo, GLuint lightMap = 0
    );

private:
    glm::mat4 calculateModelMatrix();
};

