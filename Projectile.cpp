#include "Projectile.h"

Projectile::Projectile(Material *material, glm::vec3 position, glm::vec3 movementVector) {
    this->movementVector = movementVector;

    object3D = Object3D::makeCube(
        material,
        position,
        glm::vec3(0.3f)
    );
    velocityZ = 1.0f;
}

void Projectile::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    object3D.draw(viewMatrix, projectionMatrix);
}

void Projectile::update(float delta) {
    object3D.position += movementVector * delta;
    velocityZ -= GRAVITY * delta;
    object3D.position.z += velocityZ * delta;
}
