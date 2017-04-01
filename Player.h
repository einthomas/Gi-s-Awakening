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

    void update(float delta, const Level &level);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void shoot(const Projectile &projectile);

private:
    std::vector<Projectile> projectiles;
};
