#pragma once

#include <glm/glm.hpp>
#include <glm/common.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    double rotationSpeed;

    Camera(glm::vec3 position = glm::vec3(), glm::vec3 rotation = glm::vec3());

    glm::mat4 getMatrix();
    glm::vec3 getDirection();
};
