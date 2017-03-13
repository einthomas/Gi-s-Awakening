#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Cube.h"

class Level {
public:
    std::vector<Cube> cubes;

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
};
