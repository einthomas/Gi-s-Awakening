#pragma once

#include "Ability.h"
#include "Projectile.h"
#include "Game.h"
#include "PlatformType.h"

class TeleportProjectileAbility : public Ability {
public:
    TeleportProjectileAbility(Game &game, PlatformType *projectile);

    void executeAction() override;
    void update(float delta) override;
    void draw(const Shader &shader) override;
    void draw(RenderInfo renderInfo, ShadowInfo shadowInfo) override;

private:
    std::vector<Projectile> projectiles;
    const PlatformType *projectile;
};
