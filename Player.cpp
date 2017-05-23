#include "Player.h"

Player::Player(glm::vec3 position, glm::vec3 size) {
    this->position = position;
    this->size = size;
}

void Player::update(float delta, float gravity, Level &level) {
    // update projectiles
    for (unsigned int i = 0; i < projectiles.size(); i++) {
        if (glm::length(projectiles[i].object3D.position - position) > Projectile::DESPAWN_DISTANCE ||
            projectiles[i].isDead)
        {
            projectiles.erase(projectiles.begin() + static_cast<int>(i));
        } else {
            projectiles[i].update(delta, level);
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
        isDead = true;
        //position = level.start + glm::vec3(0.f, 0.f, 1.f);
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
    for (Platform object : level.platforms) {
        object.solveCollision(position, velocity, size, onGround);
    }
    if (onGround) {
        jumpState = JumpState::GROUNDED;
    } else if (jumpState == JumpState::GROUNDED) {
        // there's probably a better way to do this
        jumpState = JumpState::FALLING;
    }

    if (ability != nullptr) {
        ability->update(delta);
    }
}

void Player::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    for (Projectile projectile : projectiles) {
        projectile.draw(viewMatrix, projectionMatrix);
    }
    if (ability != nullptr) {
        ability->draw(viewMatrix, projectionMatrix);
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
    } else if (jumpState == JumpState::JUMPING) {
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
    } else if (jumpState == JumpState::JUMPING) {
        jumpState = JumpState::FALLING;
    }
}

void Player::setSecondAbility(Ability* ability) {
    this->ability = ability;
    hasSecondAbility = true;
}

void Player::executeAbility() {
    ability->executeAction();
}
