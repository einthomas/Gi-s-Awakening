#pragma once

#include <glm/glm.hpp>

class Material {
public:
    virtual ~Material() { }

    virtual void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix
    );
    virtual void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix,
        const glm::vec3 &cameraPosition
    );

protected:
    Material() { }
};
