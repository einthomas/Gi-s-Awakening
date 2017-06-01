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
    const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix,
    const glm::mat4 &modelMatrix, const glm::vec3 &cameraPosition,
    const glm::mat4 &lightSpaceMatrix, GLuint shadowMap,
    GLfloat lightMapScale, const glm::vec2 &lightMapPosition, GLuint lightMap
) {
    shader.use();

    shader.setMatrix4("model", modelMatrix);
    shader.setMatrix4("view", viewMatrix);
    shader.setMatrix4("projection", projectionMatrix);
    shader.setVector3f("cameraPosition", cameraPosition);

    shader.setVector3f("diffuseColor", diffuseColor);
    shader.setVector3f("specularColor", specularColor);
    shader.setFloat("glossyness", glossyness);

    shader.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
    shader.setTexture2D("shadowMap", GL_TEXTURE0, shadowMap, 0);

    shader.setFloat("lightMapScale", lightMapScale);
    shader.setVector2f("lightMapPosition", lightMapPosition);
    shader.setTexture2D("lightMap", GL_TEXTURE1, lightMap, 1);
}

