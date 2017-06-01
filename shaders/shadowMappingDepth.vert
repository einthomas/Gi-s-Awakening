#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec4 vertPosition;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
    vertPosition = gl_Position;
}
