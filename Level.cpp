#include "Level.h"

#include <algorithm>

void Level::draw(const glm::mat4 &viewMatrix) {
    std::for_each(cubes.begin(), cubes.end(), [=](Cube &cube) {
        cube.draw(viewMatrix);
    });
}
