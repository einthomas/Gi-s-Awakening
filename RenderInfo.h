#pragma once

#include <glm/glm.hpp>

struct RenderInfo {

    glm::mat4 &viewMatrix;
    glm::mat4 &projectionMatrix;
    glm::vec3 &cameraPosition;

    RenderInfo(
        glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix,
        glm::vec3 &cameraPosition
    ) :
        viewMatrix(viewMatrix),
        projectionMatrix(projectionMatrix),
        cameraPosition(cameraPosition)
    {
    }
};
