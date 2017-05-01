#include "Platform.h"
#include <algorithm>

Platform::Platform(const PlatformType *type, Material *material, glm::vec3 position, std::string name) :
    Object3D{material, position, glm::vec3(1), type->mesh}, type(type), name(name), isVisible(true)
{
}

bool Platform::intersects(const glm::vec3 &position, const glm::vec3 &scale) {
    if (!isVisible) {
        return false;
    }
    glm::vec3 sumSize = (scale + this->type->size) * 0.5f;
    glm::vec3 delta = position - this->position;
    return glm::all(glm::lessThan(glm::abs(delta), sumSize));
}

void Platform::solveCollision(glm::vec3 &position, glm::vec3 &velocity, const glm::vec3 &scale, bool &onGround) const {
    if (!isVisible) {
        return;
    }

    // colliding two boxes is equivalent to
    // colliding a point with a box of the size of both boxes combined
    glm::vec3 sumSize = (scale + this->type->size) * 0.5f;
    glm::vec3 delta = position - this->position;

    // signed distance from point to closest surface
    glm::vec3 distance = glm::abs(delta) - sumSize;

    // find dimension with largest distance
    int dimension;
    if (distance.x > distance.y) {
        if (distance.x > distance.z) {
            dimension = 0;
        } else {
            dimension = 2;
        }
    } else {
        if (distance.y > distance.z) {
            dimension = 1;
        } else {
            dimension = 2;
        }
    }

    if (distance[dimension] < 0) {
        // fix intersection
        if (delta[dimension] > 0) {
            if (dimension == 2) {
                onGround = true;
            }
            position[dimension] -= distance[dimension];
            velocity[dimension] = std::max(velocity[dimension], 0.f);
        } else {
            position[dimension] += distance[dimension];
            velocity[dimension] = std::min(velocity[dimension], 0.f);
        }
    }
}

