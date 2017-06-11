#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "RenderInfo.h"
#include "ShadowInfo.h"

class Material {
public:
    virtual ~Material() { }

    virtual void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix
    );
    virtual void bind(RenderInfo renderInfo, ShadowInfo shadowInfo, const glm::mat4& modelMatrix);

protected:
    Material() { }
};
