#include "Projectile.h"

const int Projectile::DESPAWN_DISTANCE;
const float Projectile::DEATH_TIMER_START = 0.5f;

Projectile::Projectile(PlatformMaterial* material, glm::vec3 position, glm::vec3 movementVector,
    Mesh mesh
) :
    blinnMaterial(material),
    object3D(Object3D(
        blinnMaterial, position,
        glm::vec3(1.0f), glm::vec3(0.2f), mesh
    ))
{
    this->movementVector = movementVector;

    originalColor = blinnMaterial->diffuseColor / deathTimer;
    velocityZ = 1.0f;

    object3D.rotation = glm::vec3(
        (rand() % 128) / 128.f * glm::pi<float>(),
        (rand() % 128) / 128.f * glm::pi<float>(),
        (rand() % 128) / 128.f * glm::pi<float>()
    );
}

Projectile::Projectile(const Projectile &projectile) :
    blinnMaterial(projectile.blinnMaterial), object3D(Object3D(projectile.object3D))
{
    object3D.material = blinnMaterial;
    originalColor = projectile.originalColor;
    velocityZ = projectile.velocityZ;
    movementVector = glm::vec3(projectile.movementVector);
}

void Projectile::draw(const Shader& shader) {
    object3D.draw(shader);
}

void Projectile::draw(RenderInfo renderInfo, ShadowInfo shadowInfo) {
    object3D.draw(renderInfo, shadowInfo);
}

void Projectile::update(float delta, Level &level) {
    if (!particlesSpawned && isDying && !isDead && deathTimer < DEATH_TIMER_START * 0.1f) {
        // spawn particles
        glm::vec3 planeNormalVector = -glm::normalize(movementVector);
        glm::vec3 vectorWithinPlane(
            1.0f,
            1.0f,
            (-planeNormalVector.x - planeNormalVector.y) / planeNormalVector.z
        );
        ParticleSystem::beginParticleGroup(planeNormalVector, vectorWithinPlane);
        const float ANGLE_STEP = glm::pi<float>() / 60.0f;
        for (float angle = 0.0f; angle < glm::pi<float>() * 2.0f; angle += ANGLE_STEP) {
            glm::vec3 particleVector = vectorWithinPlane;
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, planeNormalVector);
            particleVector = rotationMatrix * glm::vec4(particleVector, 1.0f);
            particleVector = glm::normalize(particleVector) * 0.1f;
            ParticleSystem::makeParticle(object3D.position + particleVector);
            particleVector = glm::normalize(particleVector) * 0.05f;
            ParticleSystem::makeParticle(object3D.position + particleVector);
        }
        particlesSpawned = true;
    } else if (!isDying) {
        if (level.intersects(object3D.position, object3D.scale)) {
            SoundEngine::play2D(SoundEngine::PROJECTILE_DISAPPEAR_SOUND);
            isDying = true;
        }
    }

    if (!isDead) {
        if (isDying) {
            deathTimer -= delta;
            blinnMaterial->diffuseColor -= originalColor * delta;
            isDead = deathTimer <= 0.0f;
        } else {
            object3D.position += movementVector * delta;
            velocityZ -= GRAVITY * delta;
            object3D.position.z += velocityZ * delta;
            object3D.rotation.x += 2.0f * delta;
            object3D.rotation.y += 1.5f * delta;
        }
    }
}
