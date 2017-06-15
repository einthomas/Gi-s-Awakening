#pragma once

#include <glm/glm.hpp>

#include "BlinnMaterial.h"

class PlatformMaterial : public BlinnMaterial {
public:
    GLuint colorTexture;
    GLuint linesTexture;

    PlatformMaterial(GLuint colorTexture, GLuint linesTexture);

    void bind(
        RenderInfo renderInfo, ShadowInfo shadowInfo,
        const glm::mat4& modelMatrix,
        GLfloat lightMapScale, const glm::vec2 &lightMapPosition,
        GLuint lightMap
    ) override;
};

