#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/gl.h>

class PlatformType {
public:
    glm::vec3 size;
    GLuint VAO, elementCount;
};
