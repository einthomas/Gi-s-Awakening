#include "Player.h"

Player::Player(glm::vec3 position, glm::vec3 size) {
    this->position = position;
    this->size = size;
}

void Player::update(float delta, float gravity, glm::vec2 movement, const Level &level) {
    // update projectiles
    for (unsigned short i = 0; i < projectiles.size(); i++) {
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

    // jump
    if (jumpState == JumpState::FALLING) {
        // integral of a * x = a * x^2 / 2
        position.z += velocity.z * delta - 0.5 * gravity * delta * delta;
        velocity.z -= gravity * delta;
    }

    // check whether player fell out of map
    if (position.z < -10) { // TODO: unhardcode bottom of map
        position = level.start + glm::vec3(0.f, 0.f, 1.f);
    }

    // movement
    // exact velocity calculation deemed unnecessary
    velocity += glm::vec3(movement, 0) * movementSpeed * delta;
    // integral of a^x = (a^x-1)/log(a)
    position.x += velocity.x * (std::pow(movementDampening, delta) - 1) * movementDampeningFactor;
    position.y += velocity.y * (std::pow(movementDampening, delta) - 1) * movementDampeningFactor;
    velocity.x *= std::pow(movementDampening, delta);
    velocity.y *= std::pow(movementDampening, delta);

    // check for and handle player intersection
    onGround = false;
    for (Object3D object : level.objects) {
        object.solveCollision(position, velocity, size, onGround);
    }
    if (onGround) {
        jumpState = JumpState::GROUNDED;
    } else if (jumpState == JumpState::GROUNDED) {
        // there's probably a better way to do this
        jumpState = JumpState::FALLING;
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

void Player::jumpPressed(float delta) {
    // TODO: should use the fraction of delta at which maxJumpAccelerationDuration is reached
    if (jumpState == JumpState::GROUNDED && releasedJumpButton) {
        releasedJumpButton = false;
        jumpState = JumpState::JUMPING;
        velocity.z = jumpSpeed;
        position.z += jumpSpeed * delta;
        jumpAccelerationDuration = 0;
    }
    else if (jumpState == JumpState::JUMPING) {
        position.z += jumpSpeed * delta;
        jumpAccelerationDuration += delta;
        if (jumpAccelerationDuration > maxJumpAccelerationDuration) {
            jumpState = JumpState::FALLING;
        }
    }
}

void Player::jumpReleased() {
    releasedJumpButton = true;
    if (jumpState == JumpState::GROUNDED) {
        velocity.z = 0;
    }
    else if (jumpState == JumpState::JUMPING) {
        jumpState = JumpState::FALLING;
    }
}
