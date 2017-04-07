#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include <iostream>

#include <ft2build.h>
#include <freetype/ftlcdfil.h>
#include <freetype/config/ftoption.h>
#include FT_FREETYPE_H

#include "Shader.h"

struct Character {
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
    glm::ivec2 pixelSize;
};

class TextRenderer {
public:
    static void init(GLuint screenWidth, GLuint screenHeight, std::string fontFile, Shader shader);
    static void renderText(std::string text, GLfloat x, GLfloat y, GLfloat size, glm::vec3 color);
    static glm::vec3 calcDimensions(std::string text, GLfloat size);

private:
    static std::map<GLchar, Character> characters;
    static glm::mat4 projection;
    static GLuint VBO, VAO;
    static Shader shader;
};
