#pragma once

#include "Object3D.h"

class Projectile {
public:
    static const int DESPAWN_DISTANCE = 30.0f;
    Object3D object3D;

    Projectile(Material *material, glm::vec3 position, glm::vec3 movementVector);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void update(float delta);

    Projectile &operator=(const Projectile &otherProjectile) {
        if (this != &otherProjectile) {
            object3D = otherProjectile.object3D;
            movementVector = otherProjectile.movementVector;
            velocityZ = otherProjectile.velocityZ;
        }

        return *this;
    }

private:
    const float GRAVITY = 6.0f;
    glm::vec3 movementVector;
    float velocityZ;
};
