#pragma once

#include <GL/glew.h>

class Mesh {
public:
    GLuint VAO, elementCount;

    static Mesh fromFile(const char *filename);

    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(elementCount));
        glBindVertexArray(0);
    }

    // TODO: call glDeleteVertexArrays at some point
};
