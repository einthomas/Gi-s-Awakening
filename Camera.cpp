#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 rotation) :
    position(position), rotation(rotation)
{
}

glm::mat4 Camera::getMatrix() {
    glm::mat4 matrix;
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(-1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, -1.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, -1.0f));
    matrix = glm::translate(matrix, -position);
    return matrix;
}

glm::vec3 Camera::getDirection() {
    return glm::normalize(glm::vec3(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f) * getMatrix()));
}
