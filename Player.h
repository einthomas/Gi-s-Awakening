#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "Projectile.h"
#include "Level.h"
#include "ParticleSystem.h"

class Player {
public:
    glm::vec3 position;
    glm::vec3 size;
    glm::vec2 movement;
    bool isDead = false;
    AbilityType secondAbility;

    Player(glm::vec3 position, glm::vec3 size);

    void update(float delta, float gravity, Level &level);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void shoot(const Projectile &projectile);
    void jumpPressed(float delta);
    void jumpReleased();
    void setSecondAbility(AbilityType secondAbility);

private:
    const float movementSpeed = 54.0f;
    const float jumpSpeed = 3.5f;
    const float maxJumpAccelerationDuration = 0.3f;
    const float movementDampening = 1.2e-6f;
    const float movementDampeningFactor = 1 / std::log(movementDampening);

    enum struct JumpState {
        GROUNDED, JUMPING, FALLING
    } jumpState = JumpState::FALLING;

    glm::vec3 velocity;
    float jumpAccelerationDuration = 0.0f;
    bool releasedJumpButton = false;
    bool onGround;
    std::vector<Projectile> projectiles;

    bool hasSecondAbility = false;
};
