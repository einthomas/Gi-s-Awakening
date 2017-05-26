#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Material {
public:
    virtual ~Material() { }

    virtual void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix
    );
    virtual void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix,
        const glm::vec3 &cameraPosition, const glm::mat4 &lightSpaceMatrix, const GLuint shadowMap
    );

protected:
    Material() { }
};
