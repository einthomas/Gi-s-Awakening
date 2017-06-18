#include "Game.h"

Game::Game(Level &level, PlatformType *projectile) :
    camera(Camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(glm::radians(90.0f), 0.0f, level.startOrientation))),
    level(level),
    player(Player(level.start + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.5f, 0.5f, 2.0f))),
    projectile(projectile)
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
                    player.setSecondAbility(
                        new TeleportProjectileAbility(*this, projectile)
                    );
                }
            } else {
                level.pressurePlates[i].unHighlight();
            }
        }
    }
    player.movement = glm::vec2(0.0f);

    camera.position = player.position + glm::vec3(0.0f, 0.0f, 0.5f);
}

void Game::draw(const Shader& shader) {
    level.draw(shader);
    player.draw(shader);
}

void Game::draw(
    glm::mat4 &projectionMatrix, const glm::vec3 viewFrustumNormals[], const float ds[],
    ShadowInfo shadowInfo
) {
    glm::mat4 cameraMatrix = camera.getMatrix();
    RenderInfo renderInfo(cameraMatrix, projectionMatrix, camera.position, viewFrustumNormals, ds);
    level.draw(renderInfo, shadowInfo);
    player.draw(renderInfo, shadowInfo);
}

void Game::forwardPressed() {
    player.move(glm::vec2(
        -std::sin(camera.rotation.z),
        std::cos(camera.rotation.z)
    ));
}

void Game::leftPressed() {
    player.move(glm::vec2(
        -std::cos(camera.rotation.z),
        -std::sin(camera.rotation.z)
    ));
}

void Game::backwardsPressed() {
    player.move(glm::vec2(
        std::sin(camera.rotation.z),
        -std::cos(camera.rotation.z)
    ));
}

void Game::rightPressed() {
    player.move(glm::vec2(
        std::cos(camera.rotation.z),
        std::sin(camera.rotation.z)
    ));
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

        // TODO: use PlatformType of projectile
        player.shoot(Projectile(
            new PlatformMaterial(
                projectile->colorTexture, projectile->linesTexture,
                glm::vec3(0.635f, 0.773f, 0.98f)
            ),
            player.position + player.size / 4.0f + cameraDirection * 0.5f - glm::vec3(0.15f, 0.15f, 0.0f),
            cameraDirection * player.projectileSpeed, projectile->mesh
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
