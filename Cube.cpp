#include "Cube.h"

GLuint Cube::VAO;
GLuint Cube::VBO;
GLuint Cube::EBO;

static GLfloat vertices[] = {
    -1,-1,-1,
     1,-1,-1,
    -1, 1,-1,
     1, 1,-1,

    -1,-1, 1,
     1,-1, 1,
    -1, 1, 1,
     1, 1, 1,
};

static GLuint indices[] = {
    0, 3, 1,
    0, 2, 3,

    4, 5, 7,
    4, 7, 6,

    0, 1, 5,
    0, 5, 4,

    2, 7, 3,
    2, 6, 7,

    0, 6, 2,
    0, 4, 6,

    1, 3, 7,
    1, 7, 5,
};

Cube::Cube(Shader shader, glm::vec3 color, glm::mat4 projectionMatrix, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) :
    position(position), scale(scale), rotation(rotation), color(color), projectionMatrix(projectionMatrix), shader(shader)
{
	updateModelMatrix();
}

void Cube::draw() {
	shader.use();
	shader.setMatrix4("model", modelMatrix);
	shader.setMatrix4("projection", projectionMatrix);
	shader.setVector3f("color", color);

	glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Cube::updateModelMatrix() {
	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, scale);
}

void Cube::init() {
	// create and bind VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create and bind VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// create and bind EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// copy indices array to EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// copy vertices array to VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// unbind VAO
	glBindVertexArray(0);
}
