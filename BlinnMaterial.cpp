#include "BlinnMaterial.h"
#include "Object3D.h"

Shader BlinnMaterial::shader;

void BlinnMaterial::init() {
    shader = Shader("shaders/shader.vert", "shaders/shader.frag");
}

BlinnMaterial::BlinnMaterial(
    const glm::vec3 &diffuseColor, const glm::vec3 &specularColor,
    float glossyness
) :
    diffuseColor(diffuseColor), specularColor(specularColor),
    glossyness(glossyness)
{
}

void BlinnMaterial::bind(
    RenderInfo renderInfo, ShadowInfo shadowInfo, const glm::mat4& modelMatrix,
    GLfloat lightMapScale, const glm::vec2 &lightMapPosition, GLuint lightMap
) {
    shader.use();

    shader.setMatrix4("model", modelMatrix);
    shader.setMatrix4("view", renderInfo.viewMatrix);
    shader.setMatrix4("projection", renderInfo.projectionMatrix);
    shader.setVector3f("cameraPosition", renderInfo.cameraPosition);
    shader.setVector3f("diffuseColor", diffuseColor);
    shader.setVector3f("specularColor", specularColor);
    shader.setFloat("glossyness", glossyness);
    if (shadowInfo.numShadowMaps == 0) {
        shader.setInteger("shadowsActivated", 0);
    } else {
        shader.setInteger("shadowsActivated", 1);
    }
    for (int i = 0; i < shadowInfo.numShadowMaps; i++) {
        shader.setMatrix4("lightSpaceMatrices[" + std::to_string(i) + "]", shadowInfo.lightSpaceMatrices[i]);
        shader.setTexture2D("shadowMaps[" + std::to_string(i) + "]", GL_TEXTURE0 + i, shadowInfo.shadowMaps[i], i);
        shader.setFloat("cascadeEndsClipSpace[" + std::to_string(i) + "]", shadowInfo.cascadeEndsClipSpace[i]);
    }

    shader.setFloat("lightMapScale", lightMapScale);
    shader.setVector2f("lightMapPosition", lightMapPosition);
    shader.setTexture2D(
        "lightMap", GL_TEXTURE0 + shadowInfo.numShadowMaps,
        lightMap, shadowInfo.numShadowMaps
    );

    BlinnMaterial::shader.setFloat("invisibility", 0.0f);
}

