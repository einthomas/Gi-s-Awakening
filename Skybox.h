#pragma once

#include <vector>

#include "Object3D.h"
#include "Shader.h"

class Skybox {
public:
    Skybox(const std::vector<std::string> &textures);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

private:
    Shader shader;
    Object3D object3D;
};
