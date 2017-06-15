#include "Trigger.h"

Trigger::Trigger(
    const PlatformType *type, BlinnMaterial blinnMaterial,
    int lightMapSize, int lightMapIndex,
    glm::vec3 position, bool isTriggered,
    std::vector<Platform*> triggeredPlatforms
) :
    Platform(
        type, position, "", lightMapSize, lightMapIndex
    ),
    isTriggered(isTriggered),
    blinnMaterial(blinnMaterial),
    triggeredPlatforms(triggeredPlatforms),
    rotation(0.0f), rotationAxis(0.0f, 0.0f, 1.0f),
    originalColor(blinnMaterial.diffuseColor),
    activatedColor(glm::vec3(0.0f, 1.0f, 0.0f))
{
    if (isTriggered) {
        this->blinnMaterial.specularColor = activatedColor;
    }
    material = &this->blinnMaterial;
}

Trigger::Trigger(const Trigger &trigger) :
    Platform(trigger), isTriggered(trigger.isTriggered),
    blinnMaterial(trigger.blinnMaterial),
    rotation(0.0f), originalColor(trigger.originalColor),
    activatedColor(trigger.activatedColor)
{
    material = &blinnMaterial;
    triggeredPlatforms = std::vector<Platform*>(trigger.triggeredPlatforms);
}

void Trigger::update(float delta) {
    // TODO: rotate -> Implement rotation in Object3D where the model matrix is updated
    if (isTriggered) {
        rotation += delta;
    }
}

void Trigger::trigger() {
    isTriggered = !isTriggered;
    blinnMaterial.diffuseColor =
        isTriggered ? activatedColor : originalColor;
    blinnMaterial.specularColor =
        isTriggered ? activatedColor : glm::vec3(0.0f);
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
