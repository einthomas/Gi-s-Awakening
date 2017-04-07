#pragma once

#include <string>
#include <glm/glm.hpp>
#include <gl.h>

class PlatformType {
public:
    PlatformType();

    std::string name;
    glm::vec3 size;
    GLuint VAO;
};

#endif // PLATFORMTYPE_H
