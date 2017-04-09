#pragma once

#include "Object3D.h"
#include "PlatformType.h"

class Platform : public Object3D {
    const PlatformType *type;

public:
    Platform(const PlatformType *type, Material *material, glm::vec3 position);

    void solveCollision(glm::vec3 &position, glm::vec3 &velocity, const glm::vec3 &scale, bool &onGround) const;
};
