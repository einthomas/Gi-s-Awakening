#pragma once
#include <glm/mat4x2.hpp>

class Game;

class Ability {
public:
    Ability(Game &game);

    virtual void executeAction() = 0;
    virtual void update(float delta) = 0;
    virtual void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) = 0;

protected:
    Game &game;
};
