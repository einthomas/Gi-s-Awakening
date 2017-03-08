#pragma once

#include "Shader.h"

class Cube {
public:
	Cube(Shader shader, glm::vec3 color, glm::mat4 projectionMatrix, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	static void init();
	void draw();

    glm::vec3 position, scale, rotation, color;
    glm::mat4 projectionMatrix, modelMatrix;

    void updateModelMatrix();

private:
	static GLuint VAO, VBO, EBO;
    Shader shader;
};
