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
    void draw(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::vec3& cameraPosition,
        const glm::mat4 &lightSpaceMatrix, const GLuint shadowMap
    ) override;

private:
    std::vector<Projectile> projectiles;
};
