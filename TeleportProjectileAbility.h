#pragma once

#include "Ability.h"
#include "Projectile.h"
#include "Game.h"

#include <iostream>

class TeleportProjectileAbility : public Ability {
public:
    TeleportProjectileAbility(Game &game);

    void executeAction() override;
    void update(float delta) override;
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

private:
    std::vector<Projectile> projectiles;
};
