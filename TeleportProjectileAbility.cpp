#include "TeleportProjectileAbility.h"

TeleportProjectileAbility::TeleportProjectileAbility(Game &game, PlatformType* projectile) :
    Ability(game), projectile(projectile)
{
}

void TeleportProjectileAbility::executeAction() {
    glm::vec3 cameraDirection = game.camera.getDirection();
    projectiles.push_back(Projectile(
        new PlatformMaterial(
            projectile->colorTexture, projectile->linesTexture,
            glm::vec3(0.635f, 0.773f, 0.98f)
        ),
        game.player.position + game.player.size / 4.0f + cameraDirection * 0.5f - glm::vec3(0.15f, 0.15f, 0.0f),
        cameraDirection * game.player.projectileSpeed, projectile->mesh
    ));
}

void TeleportProjectileAbility::update(float delta) {
    for (unsigned int i = 0; i < projectiles.size(); i++) {
        if (
            glm::length(
                projectiles[i].object3D.position - game.player.position
            ) > Projectile::DESPAWN_DISTANCE ||
            projectiles[i].isDead
        ) {
            projectiles.erase(projectiles.begin() + static_cast<int>(i));
        } else {
            projectiles[i].update(delta, game.level);
            if (projectiles[i].isDead) {
                game.player.position = projectiles[i].object3D.position;
                game.player.position.z += 1.5f;
            }
        }
    }
}

void TeleportProjectileAbility::draw(const Shader& shader) {
    for (Projectile projectile : projectiles) {
        projectile.draw(shader);
    }
}

void TeleportProjectileAbility::draw(RenderInfo renderInfo, ShadowInfo shadowInfo) {
    for (Projectile projectile : projectiles) {
        projectile.draw(renderInfo, shadowInfo);
    }
}
