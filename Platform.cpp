#include "Platform.h"

Platform::Platform(PlatformType *type, glm::vec3 position) {
    this->position = position;
    this->type = type;
    this->VAO = type->VAO;
}

