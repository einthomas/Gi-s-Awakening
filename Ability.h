#pragma once
#include <glm/mat4x2.hpp>
#include "Shader.h"
#include "RenderInfo.h"
#include "ShadowInfo.h"

class Game;

class Ability {
public:
    Ability(Game &game);
    virtual ~Ability() = default;

    virtual void executeAction() = 0;
    virtual void update(float delta) = 0;
    virtual void draw(const Shader &shader) = 0;
    virtual void draw(RenderInfo renderInfo, ShadowInfo shadowInfo) = 0;

protected:
    Game &game;
};
