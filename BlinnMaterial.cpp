#include "BlinnMaterial.h"

Shader BlinnMaterial::shader;

void BlinnMaterial::init() {
    shader = Shader("shaders/shader.vert", "shaders/shader.frag");
}

BlinnMaterial::BlinnMaterial(const glm::vec3 &diffuseColor, const glm::vec3 &specularColor, float glossyness) :
	diffuseColor(diffuseColor), specularColor(specularColor), glossyness(glossyness)
{
}

void BlinnMaterial::bind(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix) {
    shader.use();
    shader.setMatrix4("model", modelMatrix);
    shader.setMatrix4("view", viewMatrix);
    shader.setMatrix4("projection", projectionMatrix);
    shader.setVector3f("diffuseColor", diffuseColor);
}

