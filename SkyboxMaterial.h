#pragma once

#include <vector>

#include "Material.h"
#include "Shader.h"

class SkyboxMaterial : public Material {
public:
    static void init();

    SkyboxMaterial(const std::vector<std::string> &textures);
    ~SkyboxMaterial() { }

    void bind(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix) override;

private:
    static Shader shader;

    GLuint cubeMap;

    GLuint loadCubemap(const std::vector<std::string> &textures);
};
