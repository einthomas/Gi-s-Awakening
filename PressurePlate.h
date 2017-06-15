#pragma once
#include "Platform.h"
#include "PlatformMaterial.h"
#include "AbilityType.h"

class PressurePlate : public Platform {
public:
    AbilityType abilityType;

    PressurePlate(
        const PlatformType *type, PlatformMaterial blinnMaterial,
        int lightMapSize, int lightMapIndex,
        glm::vec3 position, AbilityType abilityType
    );
    PressurePlate(const PressurePlate &pressurePlate);
    bool steppedOn(const glm::vec3 &playerPosition, const glm::vec3 &playerSize);
    void highlight();
    void unHighlight();

private:
    PlatformMaterial blinnMaterial;
    glm::vec3 originalDiffuseColor;
    glm::vec3 highlightDiffuseColor;
};
