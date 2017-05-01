#include "Trigger.h"

Trigger::Trigger(const PlatformType *type, Material *material, glm::vec3 position, bool isTriggered, std::vector<Platform*> triggeredPlatforms) :
    Platform(type, material, position, ""), isTriggered(isTriggered), triggeredPlatforms(triggeredPlatforms), rotation(0.0f), rotationAxis(0.0f, 0.0f, 1.0f)
{
}

void Trigger::update(float delta) {
    // TODO: rotate -> Implement rotation in Object3D where the model matrix is updated
    if (isTriggered) {
        rotation += delta;
    }
}

void Trigger::trigger() {
    isTriggered = !isTriggered;
    for (Platform *triggeredPlatform : triggeredPlatforms) {
        triggeredPlatform->isVisible = isTriggered;
    }
}

bool Trigger::intersects(const glm::vec3 &position, const glm::vec3 &scale) {
    if (Platform::intersects(position, scale)) {
        trigger();
        return true;
    }
    
    return false;
}
