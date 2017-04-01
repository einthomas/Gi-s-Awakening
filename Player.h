#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "Projectile.h"
#include "Level.h"

class Player {
public:
    glm::vec3 position;
    glm::vec3 size;

    Player(glm::vec3 position, glm::vec3 size);

    void update(float delta, float gravity, const Level &level);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void shoot(const Projectile &projectile);
    void jumpPressed(float delta);
    void jumpReleased();

private:
    enum struct JumpState {
        GROUNDED, JUMPING, FALLING
    } jumpState = JumpState::FALLING;

    float velocityZ = 0.0f;
    float jumpSpeed = 5.0f;
    float maxJumpAccelerationDuration = 0.3f;
    float jumpAccelerationDuration = 0.0f;
    bool releasedJumpButton = false;
    bool onGround;
    std::vector<Projectile> projectiles;
};
