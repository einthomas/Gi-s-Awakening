#pragma once

#include "Object3D.h"
#include "PlatformType.h"

class Platform : public Object3D {
    const PlatformType *type;

public:
    const std::string name;

    bool isVisible;

    Platform(const PlatformType *type, Material *material, glm::vec3 position, std::string name);

    virtual bool intersects(const glm::vec3 &position, const glm::vec3 &scale);
    void solveCollision(glm::vec3 &position, glm::vec3 &velocity, const glm::vec3 &scale, bool &onGround) const;
};
