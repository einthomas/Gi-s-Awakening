#pragma once

#include <glm/glm.hpp>

#include "Material.h"
#include "Shader.h"

#include "Camera.h"

class BlinnMaterial : public Material {
public:
    glm::vec3 diffuseColor, specularColor;
    float glossyness;

    static void init();

    BlinnMaterial(const glm::vec3 &diffuseColor, const glm::vec3 &specularColor, float glossyness);
    ~BlinnMaterial() { }

    void bind(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix,
        const glm::vec3 &cameraPosition
    ) override;

private:
    static Shader shader;
};
