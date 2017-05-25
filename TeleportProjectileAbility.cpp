#include "TeleportProjectileAbility.h"

TeleportProjectileAbility::TeleportProjectileAbility(Game &game) :
    Ability(game)
{
}

void TeleportProjectileAbility::executeAction() {
    glm::vec3 cameraDirection = game.camera.getDirection();
    projectiles.push_back(Projectile(
        BlinnMaterial(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), 0.0f),
        game.player.position + game.player.size / 4.0f + cameraDirection * 0.5f - glm::vec3(0.15f, 0.15f, 0.0f),
        cameraDirection * game.player.projectileSpeed
    ));
}

void TeleportProjectileAbility::update(float delta) {
    for (unsigned int i = 0; i < projectiles.size(); i++) {
        if (glm::length(projectiles[i].object3D.position - game.player.position) > Projectile::DESPAWN_DISTANCE ||
            projectiles[i].isDead)
        {
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

void TeleportProjectileAbility::draw(
    const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix,
    const glm::vec3& cameraPosition
) {
    for (Projectile projectile : projectiles) {
        projectile.draw(viewMatrix, projectionMatrix, cameraPosition);
    }
}
