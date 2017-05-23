#include "Mesh.h"

#include <fstream>
#include <vector>

Mesh::Mesh(GLuint VAO, GLuint elementCount) :
    VAO(VAO), elementCount(elementCount)
{
}

Mesh Mesh::fromFile(const char *filename) {
    std::ifstream vboFile(filename, std::ifstream::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(vboFile)), std::istreambuf_iterator<char>());

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(buffer.size()), buffer.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return Mesh(VAO, buffer.size() / 8 / 4);
}

void Mesh::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(elementCount));
    glBindVertexArray(0);
}
