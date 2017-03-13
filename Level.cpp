#include "Level.h"

#include <algorithm>

void Level::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    std::for_each(objects.begin(), objects.end(), [=](Object3D &object) {
        object.draw(viewMatrix, projectionMatrix);
    });
}
