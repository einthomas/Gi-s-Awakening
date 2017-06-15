#pragma once

#include <glm/glm.hpp>

struct RenderInfo {

    const glm::mat4 &viewMatrix;
    const glm::mat4 &projectionMatrix;
    const glm::vec3 &cameraPosition;
    const glm::vec3 *viewFrustumNormals;
    const float *ds;

    RenderInfo(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix,
        const glm::vec3 &cameraPosition, const glm::vec3 viewFrustumNormals[],
        const float ds[]
    ) :
        viewMatrix(viewMatrix),
        projectionMatrix(projectionMatrix),
        cameraPosition(cameraPosition),
        viewFrustumNormals(viewFrustumNormals),
        ds(ds)
    {
    }
};
