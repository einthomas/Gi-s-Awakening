#pragma once

#include "Object3D.h"

class Projectile : public Object3D {
public:
    Projectile(Material *material, glm::vec3 position, glm::vec3 movementVector);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void update(float delta);

private:
    const float GRAVITY = 6.0f;
    Object3D object3D;
    glm::vec3 movementVector;
    float velocityZ;
};
