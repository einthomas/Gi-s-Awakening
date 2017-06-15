#include "PressurePlate.h"

PressurePlate::PressurePlate(
    const PlatformType *type, PlatformMaterial blinnMaterial,
    int lightMapSize, int lightMapIndex,
    glm::vec3 position, AbilityType abilityType
) :
    Platform(
        type, position, "", lightMapSize, lightMapIndex
    ),
    abilityType(abilityType),
    blinnMaterial(blinnMaterial),
    originalDiffuseColor(blinnMaterial.diffuseColor),
    highlightDiffuseColor(originalDiffuseColor * 2.0f)
{
    material = &this->blinnMaterial;
}

PressurePlate::PressurePlate(const PressurePlate &pressurePlate) :
    Platform(pressurePlate), abilityType(pressurePlate.abilityType),
    blinnMaterial(pressurePlate.blinnMaterial),
    originalDiffuseColor(blinnMaterial.diffuseColor),
    highlightDiffuseColor(originalDiffuseColor * 2.0f)
{
    material = &blinnMaterial;
}

bool PressurePlate::steppedOn(const glm::vec3 &playerPosition, const glm::vec3 &playerSize) {
    // assuming that every pressure plate is a square
    return glm::length(playerPosition - glm::vec3(0.0f, 0.0f, playerSize.z / 2.0f) - position) < size.x;
}

void PressurePlate::highlight() {
    blinnMaterial.diffuseColor = highlightDiffuseColor;
}

void PressurePlate::unHighlight() {
    blinnMaterial.diffuseColor = originalDiffuseColor;
}
