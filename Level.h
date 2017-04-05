#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Object3D.h"

class Level {
public:
    std::vector<Object3D> objects;
    glm::vec3 start, end;
    float startOrientation;

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

    static Level fromFile(const char *filename, Material *material);
};
