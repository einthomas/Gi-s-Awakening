#pragma once

#include <GL/glew.h>

class Mesh {
public:
    GLuint VAO, elementCount;

    Mesh(GLuint VAO, GLuint elementCount);

    static Mesh fromFile(const char *filename);

    void draw();

    // TODO: call glDeleteVertexArrays at some point
};
