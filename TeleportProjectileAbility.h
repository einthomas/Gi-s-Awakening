#pragma once

#include "Ability.h"
#include "Projectile.h"
#include "Game.h"

class TeleportProjectileAbility : public Ability {
public:
    TeleportProjectileAbility(Game &game);

    void executeAction() override;
    void update(float delta) override;
    void draw(const Shader &shader) override;
    void draw(RenderInfo renderInfo, ShadowInfo shadowInfo) override;

private:
    std::vector<Projectile> projectiles;
};
