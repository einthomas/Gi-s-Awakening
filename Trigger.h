#pragma once

#include <vector>

#include "Platform.h"
#include "BlinnMaterial.h"

class Trigger : public Platform {
public:
    bool isTriggered;

    Trigger(
        const PlatformType *type, BlinnMaterial blinnMaterial,
        int lightMapSize, int lightMapIndex,
        glm::vec3 position, bool isTriggered,
        std::vector<Platform*> triggeredPlatforms
    );
    Trigger(const Trigger &trigger);
    void update(float delta);
    void trigger();
    bool intersects(const glm::vec3 &position, const glm::vec3 &scale) override;
    BlinnMaterial blinnMaterial;

private:
    std::vector<Platform*> triggeredPlatforms;
    float rotation;
    glm::vec3 rotationAxis;
    glm::vec3 originalColor, activatedColor;
};
