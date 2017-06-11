#pragma once

#include <glm/glm.hpp>

#include "Material.h"
#include "Shader.h"
#include "RenderInfo.h"
#include "ShadowInfo.h"

class BlinnMaterial : public Material {
public:
    glm::vec3 diffuseColor, specularColor;
    float glossyness;

    static void init();

    BlinnMaterial(const glm::vec3 &diffuseColor, const glm::vec3 &specularColor, float glossyness);
    ~BlinnMaterial() { }

    void bind(RenderInfo renderInfo, ShadowInfo shadowInfo, const glm::mat4& modelMatrix) override;

private:
    static Shader shader;
};
