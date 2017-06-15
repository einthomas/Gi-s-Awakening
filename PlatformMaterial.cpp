#include "PlatformMaterial.h"

PlatformMaterial::PlatformMaterial(
    GLuint colorTexture, GLuint linesTexture
) :
    BlinnMaterial(glm::vec3(1.0f), glm::vec3(1.0f, 1.0f, 0.5f), 64.0f),
    colorTexture(colorTexture), linesTexture(linesTexture)
{ }

void PlatformMaterial::bind(
    RenderInfo renderInfo, ShadowInfo shadowInfo,
    const glm::mat4& modelMatrix,
    GLfloat lightMapScale, const glm::vec2& lightMapPosition,
    GLuint lightMap
) {
    BlinnMaterial::bind(
        renderInfo, shadowInfo, modelMatrix,
        lightMapScale, lightMapPosition, lightMap
    );

    BlinnMaterial::shader.setTexture2D(
        "colorTexture", GL_TEXTURE0 + shadowInfo.numShadowMaps + 1,
        colorTexture, shadowInfo.numShadowMaps + 1
    );
    BlinnMaterial::shader.setTexture2D(
        "linesTexture", GL_TEXTURE0 + shadowInfo.numShadowMaps + 2,
        linesTexture, shadowInfo.numShadowMaps + 2
    );
}
