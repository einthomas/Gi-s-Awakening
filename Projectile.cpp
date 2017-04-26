#include "Projectile.h"

const int Projectile::DESPAWN_DISTANCE;
const float Projectile::DEATH_TIMER_START = 0.5f;

Projectile::Projectile(BlinnMaterial material, glm::vec3 position, glm::vec3 movementVector) :
    blinnMaterial(material)
{
    this->movementVector = movementVector;

    originalColor = blinnMaterial.diffuseColor / deathTimer;
    object3D = Object3D::makeCube(
        new BlinnMaterial(blinnMaterial),
        position,
        glm::vec3(0.3f)
    );
    velocityZ = 1.0f;
}

void Projectile::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    object3D.draw(viewMatrix, projectionMatrix);
}

void Projectile::update(float delta) {
    if (!isDead) {
        if (isDying) {
            deathTimer -= delta;
            ((BlinnMaterial*)object3D.material)->diffuseColor -= originalColor * delta;
            isDead = deathTimer <= 0.0f;
        } else {
            object3D.position += movementVector * delta;
            velocityZ -= GRAVITY * delta;
            object3D.position.z += velocityZ * delta;
        }
    }
}
