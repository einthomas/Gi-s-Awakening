#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

struct ShadowInfo {

    glm::mat4 *lightSpaceMatrices;
    GLuint *shadowMaps;
    GLuint numShadowMaps;
    float *cascadeEndsClipSpace;

    ShadowInfo(
        glm::mat4 *lightSpaceMatrices, GLuint *shadowMaps,
        GLuint numShadowMaps, float *cascadeEndsClipSpace
    ) {
        this->lightSpaceMatrices = lightSpaceMatrices;
        this->shadowMaps = shadowMaps;
        this->numShadowMaps = numShadowMaps;
        this->cascadeEndsClipSpace = cascadeEndsClipSpace;
    }
};
