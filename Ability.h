#pragma once
#include <glm/mat4x2.hpp>
#include "Shader.h"

class Game;

class Ability {
public:
    Ability(Game &game);

    virtual void executeAction() = 0;
    virtual void update(float delta) = 0;
    virtual void draw(const Shader &shader) = 0;
    virtual void draw(
        const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::vec3& cameraPosition,
        const glm::mat4 &lightSpaceMatrix, const GLuint shadowMap
    ) = 0;

protected:
    Game &game;
};
