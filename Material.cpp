#include "Material.h"

void Material::bind(
    const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
    const glm::mat4& modelMatrix
) {
}

void Material::bind(
    const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix,
    const glm::mat4 &modelMatrix, const glm::vec3 &cameraPosition,
    const glm::mat4 &lightSpaceMatrix, GLuint shadowMap,
    GLfloat lightmapScale, const glm::vec2 &lightmapPosition,
    GLuint lightmap
) {
}
