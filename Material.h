#pragma once

#include <glm/glm.hpp>

class Material {
public:
    virtual ~Material() { }

    virtual void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix
    ) = 0;

protected:
    Material() { }
};
