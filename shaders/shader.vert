#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;

out vec3 vertColor;

void main() {
    gl_Position = projection * model * vec4(position, 1.0f);
    vertColor = color;
}
