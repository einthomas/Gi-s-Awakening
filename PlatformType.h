#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/gl.h>

class PlatformType {
public:
    PlatformType();

    std::string name;
    glm::vec3 size;
    GLuint VAO;
};
