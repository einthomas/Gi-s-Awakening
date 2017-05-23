#include "Game.h"

Game::Game(Level &level) :
    camera(Camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(glm::radians(90.0f), 0.0f, level.startOrientation))),
    level(level),
    player(Player(level.start + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.5f, 0.5f, 2.0f)))
{
}

void Game::update(float delta) {
    lastDelta = delta;
    if (!player.isDead) {
        player.update(delta, gravity, level);
        level.update(delta);
        ParticleSystem::update(delta);

        for (int i = 0; i < level.pressurePlates.size(); i++) {
            if (level.pressurePlates[i].steppedOn(player.position, player.size)) {
                level.pressurePlates[i].highlight();
                if (!player.hasSecondAbility && level.pressurePlates[i].abilityType == AbilityType::TELEPORT) {
                    player.setSecondAbility(new TeleportProjectileAbility(*this));
                }
            } else {
                level.pressurePlates[i].unHighlight();
            }
        }
    }
    player.movement = glm::vec2(0.0f);

    camera.position = player.position + glm::vec3(0.f, 0.f, 0.5f);
}

void Game::draw(const glm::mat4 &projectionMatrix) {
    level.draw(camera.getMatrix(), projectionMatrix);
    player.draw(camera.getMatrix(), projectionMatrix);
}

void Game::forwardPressed() {
    player.movement.x -= std::sin(camera.rotation.z);
    player.movement.y += std::cos(camera.rotation.z);
}

void Game::leftPressed() {
    player.movement.x -= std::cos(camera.rotation.z);
    player.movement.y -= std::sin(camera.rotation.z);
}

void Game::backwardsPressed() {
    player.movement.x += std::sin(camera.rotation.z);
    player.movement.y -= std::cos(camera.rotation.z);
}

void Game::rightPressed() {
    player.movement.x += std::cos(camera.rotation.z);
    player.movement.y += std::sin(camera.rotation.z);
}

void Game::confirmPressed() {
    if (player.isDead) {
        player.isDead = false;
        player.position = level.start + glm::vec3(0.f, 0.f, 2.f);
    } else {
        player.jumpPressed(lastDelta);
    }
}

void Game::confirmReleased() {
    player.jumpReleased();
}

void Game::primaryActionPressed() {
    isPrimaryActionPressed = true;
}

void Game::primaryActionReleased() {
    if (isPrimaryActionPressed) {
        isPrimaryActionPressed = false;
        glm::vec3 cameraDirection = camera.getDirection();

        player.shoot(Projectile(
            BlinnMaterial(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f),
            player.position + player.size / 4.0f + cameraDirection * 0.5f - glm::vec3(0.15f, 0.15f, 0.0f),
            cameraDirection * player.projectileSpeed
        ));
    }
}

void Game::secondaryActionPressed() {
    isSecondaryActionPressed = true;
}

void Game::secondaryActionReleased() {
    if (isSecondaryActionPressed) {
        isSecondaryActionPressed = false;
        if (player.hasSecondAbility) {
            player.executeAbility();
        }
    }
}

void Game::cursorMoved(double deltaX, double deltaY) {
    camera.rotation.z -= deltaX * camera.rotationSpeed;
    camera.rotation.x -= deltaY * camera.rotationSpeed;
    camera.rotation.x = glm::clamp(camera.rotation.x, 0.f, glm::pi<float>());
}
