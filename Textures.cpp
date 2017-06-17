#include "Textures.h"
#include "SOIL.h"

GLuint loadTexture(const char* filename, GLfloat fLargest) {
    GLuint texture;
    glGenTextures(1, &texture);

    int width, height;
    auto image = SOIL_load_image(
        filename,
        &width, &height, nullptr, SOIL_LOAD_RGB
    );
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE, image
    );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SOIL_free_image_data(image);

    return texture;
}
