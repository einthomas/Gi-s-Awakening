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
static const char *title = "Guardian's Awakening: The Mending of the Sky";

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

    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 4.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    level.objects.push_back(Object3D::makeCube(
        material.get(),
        glm::vec3(0.0f, 8.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f)
    ));

    int centerX = width / 2, centerY = height / 2;
    glfwSetCursorPos(window, centerX, centerY);

	float velocityZ = 0.0f;
    float gravity = 16.0f;
	bool onGround = true;
	float playerHeight = 2.0f;
    float movementSpeed = 4.0f;
    float rotationSpeed = 14.0f;
    float jumpSpeed = 8.0f;

    std::chrono::steady_clock clock;
    std::chrono::steady_clock::time_point previousTime;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = clock.now();
        float delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count() * 0.001f;
        previousTime = currentTime;

        // movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.position.x -= std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            camera.position.y += std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.position.x -= std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            camera.position.y -= std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.position.x += std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            camera.position.y -= std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.position.x += std::cos(glm::radians(camera.rotation.z)) * delta * movementSpeed;
            camera.position.y += std::sin(glm::radians(camera.rotation.z)) * delta * movementSpeed;
        }

        // jumping
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround) {
			onGround = false;
            velocityZ = jumpSpeed;
		}

        velocityZ -= gravity * delta;
        camera.position.z += velocityZ * delta;
		if (camera.position.z < playerHeight) {
			camera.position.z = playerHeight;
			onGround = true;
		}

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
