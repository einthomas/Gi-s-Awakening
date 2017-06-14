#pragma once

#include "Object3D.h"
#include "PlatformType.h"

class Platform : public Object3D {
public:
    const std::string name;

    bool isVisible;

    int lightmapIndex;

    Platform(
        const PlatformType *type, Material *material, glm::vec3 position,
        std::string name, int lightMapSize = 1, int lightMapIndex = 0
    );

    virtual bool intersects(const glm::vec3 &position, const glm::vec3 &scale);
    void solveCollision(
        glm::vec3 &position, glm::vec3 &velocity,
        const glm::vec3 &scale, bool &onGround
    ) const;
};
