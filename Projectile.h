#pragma once

#include "BlinnMaterial.h"
#include "Object3D.h"
#include "ParticleSystem.h"
#include "Level.h"

class Projectile {
public:
    static const int DESPAWN_DISTANCE = 30;
    Object3D object3D;
    glm::vec3 movementVector;
    bool isDying = false;
    bool isDead = false;
    static const float DEATH_TIMER_START;
    float deathTimer = 0.5f;
    bool particlesSpawned = false;

    Projectile(BlinnMaterial material, glm::vec3 position, glm::vec3 movementVector);
    Projectile(const Projectile &projectile);

    void draw(const Shader &shader);
    void draw(RenderInfo renderInfo, ShadowInfo shadowInfo);
    void update(float delta, Level &level);

private:
    constexpr static float GRAVITY = 6.0f;
    BlinnMaterial blinnMaterial;
    glm::vec3 originalColor;
    float velocityZ;
};
