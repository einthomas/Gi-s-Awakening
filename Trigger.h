#pragma once

#include <vector>

#include "Platform.h"
#include "BlinnMaterial.h"
#include "SoundEngine.h"
#include "PlatformMaterial.h"

class Trigger : public Platform {
public:
    bool isTriggered;

    Trigger(
        const PlatformType *type, PlatformMaterial blinnMaterial,
        int lightMapSize, int lightMapIndex,
        glm::vec3 position, bool isTriggered,
        std::vector<Platform*> triggeredPlatforms,
        glm::vec3 movement = glm::vec3(0)
    );
    Trigger(const Trigger &trigger);
    void update(float delta);
    void trigger();
    bool intersects(const glm::vec3 &position, const glm::vec3 &scale) override;
    PlatformMaterial blinnMaterial;

private:
    std::vector<Platform*> triggeredPlatforms;
    glm::vec3 originalColor, activatedColor;
    glm::vec3 movement;
};
