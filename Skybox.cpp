#include "Skybox.h"
#include "SkyboxMaterial.h"

Skybox::Skybox(const std::vector<std::string> &textures) :
    shader(Shader("shaders/skybox.vert", "shaders/skybox.frag")),
    object3D(Object3D::makeSkyboxCube(new SkyboxMaterial(textures), glm::vec3(0.0f), glm::vec3(1.0f)))
{
}

void Skybox::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    object3D.draw(viewMatrix, projectionMatrix);
}
