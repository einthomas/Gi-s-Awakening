#pragma once

#include "BlinnMaterial.h"
#include "Object3D.h"

class Projectile {
public:
    static const int DESPAWN_DISTANCE = 30.0f;
    Object3D object3D;
    glm::vec3 movementVector;
    bool isDying = false;
    bool isDead = false;
    static const float DEATH_TIMER_START;
    float deathTimer = 0.5f;
    bool particlesSpawned = false;

    Projectile(BlinnMaterial material, glm::vec3 position, glm::vec3 movementVector);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void update(float delta);

private:
    constexpr static float GRAVITY = 6.0f;
    BlinnMaterial blinnMaterial;
    glm::vec3 originalColor;
    float velocityZ;
};
