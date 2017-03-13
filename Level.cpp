#include "Level.h"

#include <algorithm>

void Level::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    std::for_each(cubes.begin(), cubes.end(), [=](Cube &cube) {
        cube.draw(viewMatrix, projectionMatrix);
    });
}
