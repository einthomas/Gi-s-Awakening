#include "TextRenderer.h"

std::map<GLchar, Character> TextRenderer::characters;
glm::mat4 TextRenderer::projection;
GLuint TextRenderer::VBO, TextRenderer::VAO;
Shader TextRenderer::shader;

void TextRenderer::init(GLuint screenWidth, GLuint screenHeight, std::string fontFile, Shader shader) {
    projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Library_SetLcdFilter(ft, FT_LCD_FILTER_MAX);

    FT_Face face;
    if (FT_New_Face(ft, fontFile.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 128);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph " << c << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // init VBO and VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    TextRenderer::shader = shader;
}

void TextRenderer::renderText(std::string text, GLfloat x, GLfloat y, GLfloat size, glm::vec3 color) {
    shader.use();
    shader.setVector3f("textColor", color);
    shader.setMatrix4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator iter;
    for (iter = text.begin(); iter != text.end(); iter++) {
        Character c = characters[*iter];

        GLfloat posX = x + c.bearing.x * size;
        GLfloat posY = y - (c.size.y - c.bearing.y) * size;

        GLfloat w = c.size.x * size;
        GLfloat h = c.size.y * size;

        GLfloat vertices[6][4] = {
            { posX,     posY + h,   0.0, 0.0 },
            { posX,     posY,       0.0, 1.0 },
            { posX + w, posY,       1.0, 1.0 },

            { posX,     posY + h,   0.0, 0.0 },
            { posX + w, posY,       1.0, 1.0 },
            { posX + w, posY + h,   1.0, 0.0 }
        };

        glBindTexture(GL_TEXTURE_2D, c.textureID);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (c.advance >> 6) * size; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec3 TextRenderer::calcDimensions(std::string text, GLfloat size) {
    glm::vec3 dimensions(0.0f);

    std::string::const_iterator iter;
    for (iter = text.begin(); iter != text.end(); iter++) {
        Character c = characters[*iter];
        dimensions.x += (c.advance >> 6) * size;
        GLfloat h = c.size.y * size;

        if (h > dimensions.y) {
            dimensions.y = h;
        }
    }

    return dimensions;
}
