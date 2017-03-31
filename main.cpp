#include <iostream>
#include <chrono>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Cube.h"
#include "Camera.h"
#include "Level.h"
#include "Object3D.h"
#include "BlinnMaterial.h"

static int width = 1280, height = 720;
static const char *title = "Gi's Awakening: The Mending of the Sky";

GLFWwindow *initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

bool initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    return true;
}

int main(void) {
    GLFWwindow* window = initGLFW();
    if (window == NULL) {
        return 0;
    }

    // Set OpenGL options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // activate v-sync
    glfwSwapInterval(1);

    if (!initGLEW()) {
        return 0;
    }

    glClearColor(0.2f, 0.6f, 0.8f, 1.0f);

    glViewport(0, 0, width, height);
    glm::mat4 projectionMatrix = glm::perspectiveFov(
        glm::radians(70.0f),
        static_cast<float>(width),
        static_cast<float>(height),
        0.1f, 100.0f
    );

    Camera camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(90.0f, 0.0f, 0.0f));

    BlinnMaterial::init();
    std::unique_ptr<BlinnMaterial> material(new BlinnMaterial(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f));

    Level level;

    // ground 1
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // ground 2
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 7.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // ground 3
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 14.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // ceiling
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 0.0f, 4.5f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // stair 1
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 21.0f, 1.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // wall 1
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(2.0f, 0.0f, 1.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // wall 2
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(-2.0f, 0.0f, 1.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    // wall 3
    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, -2.0f, 1.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    int centerX = width / 2, centerY = height / 2;
    glfwSetCursorPos(window, centerX, centerY);

	float velocityZ = 0.0f;
    float gravity = 12.0f;
    float movementSpeed = 4.0f;
    float rotationSpeed = 14.0f;
    float jumpSpeed = 5.0f;
    float maxJumpAccelerationDuration = 0.3f;
    float jumpAccelerationDuration = 0.f;

    enum struct JumpState {
        GROUNDED, JUMPING, FALLING
    } jumpState = JumpState::FALLING;
    bool releasedJumpButton = true; // don't allow continues jumping by keeping the key down

    glm::vec3 playerSize = glm::vec3(0.5f, 0.5f, 2.0f);
    glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 2.0f);

    std::chrono::steady_clock clock;
    auto previousTime = clock.now();

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = clock.now();
        float delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count() * 0.001f;
        previousTime = currentTime;

        // movement
        glm::vec3 potentialMovement = glm::vec3(0);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            potentialMovement.x -= std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            potentialMovement.y += std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            potentialMovement.x -= std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            potentialMovement.y -= std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            potentialMovement.x += std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            potentialMovement.y -= std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            potentialMovement.x += std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            potentialMovement.y += std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }

        playerPosition += potentialMovement;

        // jumping
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            // TODO: should use the fraction of delta at which maxJumpAccelerationDuration is reached
            if (jumpState == JumpState::GROUNDED && releasedJumpButton) {
                releasedJumpButton = false;
                jumpState = JumpState::JUMPING;
                velocityZ = jumpSpeed;
                playerPosition.z += jumpSpeed * delta;
                jumpAccelerationDuration = 0;
            } else if (jumpState == JumpState::JUMPING) {
                playerPosition.z += jumpSpeed * delta;
                jumpAccelerationDuration += delta;
                if (jumpAccelerationDuration > maxJumpAccelerationDuration) {
                    jumpState = JumpState::FALLING;
                }
            }
        } else {
            releasedJumpButton = true;
            if (jumpState == JumpState::GROUNDED) {
                velocityZ = 0;
            } else if (jumpState == JumpState::JUMPING) {
                jumpState = JumpState::FALLING;
            }
        }

        if (jumpState == JumpState::FALLING) {
            // frame-rate intependent position calculation
            playerPosition.z += velocityZ * delta - 0.5 * gravity * delta * delta;
            velocityZ -= gravity * delta;
        }

        bool onGround = false;
        for (Object3D object : level.objects) {
            playerPosition = object.solveCollision(playerPosition, playerSize, onGround);
        }
        if (onGround) {
            jumpState = JumpState::GROUNDED;
        } else if (jumpState == JumpState::GROUNDED) {
            // there's probably a better way to do this
            jumpState = JumpState::FALLING;
        }

        camera.position = playerPosition + glm::vec3(0.f, 0.f, 0.5f);
        
        // mouse look
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwSetCursorPos(window, centerX, centerY);

        camera.rotation.z -= (mouseX - centerX) * rotationSpeed * delta;
        camera.rotation.x -= (mouseY - centerY) * rotationSpeed * delta;

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        level.draw(camera.getMatrix(), projectionMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
