#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 rotation) :
    position(position), rotation(rotation)
{
}

glm::mat4 Camera::getMatrix() {
    glm::mat4 matrix;
    matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(-1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, -1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));
    matrix = glm::translate(matrix, -position);
    return matrix;
}

