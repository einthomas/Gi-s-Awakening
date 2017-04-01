#include "Player.h"

Player::Player(glm::vec3 position, glm::vec3 size) {
    this->position = position;
    this->size = size;
}

void Player::update(float delta, const Level &level) {
    for (int i = 0; i < projectiles.size(); i++) {
        if (glm::length(projectiles[i].object3D.position - position) > Projectile::DESPAWN_DISTANCE) {
            projectiles.erase(projectiles.begin() + i);
        } else {
            bool projectileIntersects = false;
            for (Object3D levelObject : level.objects) {
                if (levelObject.intersects(projectiles[i].object3D.position, projectiles[i].object3D.scale)) {
                    projectileIntersects = true;
                    projectiles.erase(projectiles.begin() + i);
                    break;
                }
            }
            if (!projectileIntersects) {
                projectiles[i].update(delta);
            }
        }
    }
}

void Player::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    for (Projectile projectile : projectiles) {
        projectile.draw(viewMatrix, projectionMatrix);
    }
}

void Player::shoot(const Projectile &projectile) {
    projectiles.push_back(projectile);
}

