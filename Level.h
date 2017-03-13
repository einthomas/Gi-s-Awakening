#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Object3D.h"

class Level {
public:
    std::vector<Object3D> objects;

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
};
