#pragma once

#include <vector>
#include <iostream>

#include "Platform.h"

class Trigger : public Platform {
public:
    bool isTriggered;

    Trigger(const PlatformType *type, Material *material, glm::vec3 position, bool isTriggered, std::vector<Platform*> triggeredPlatforms);
    void update(float delta);
    void trigger();
    bool intersects(const glm::vec3 &position, const glm::vec3 &scale) override;

private:
    std::vector<Platform*> triggeredPlatforms;
    float rotation;
    glm::vec3 rotationAxis;
};
