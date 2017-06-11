#pragma once

#include "Camera.h"
#include "Level.h"
#include "Player.h"
#include "TeleportProjectileAbility.h"

class Game {
public:
    Camera camera;
    Level &level;
    Player player;

    Game(Level &level);
    void update(float delta);
    void draw(const Shader &shader);
    void draw(glm::mat4 &projectionMatrix, ShadowInfo shadowInfo);
    void forwardPressed();
    void leftPressed();
    void backwardsPressed();
    void rightPressed();
    void confirmPressed();
    void confirmReleased();
    void primaryActionPressed();
    void primaryActionReleased();
    void secondaryActionPressed();
    void secondaryActionReleased();
    void cursorMoved(double deltaX, double deltaY);

private:
    float lastDelta;
    bool isPrimaryActionPressed = false;
    bool isSecondaryActionPressed = false;
    float gravity = 8.5f;
};
