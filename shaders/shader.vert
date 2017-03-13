#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 diffuseColor;

out vec3 vertColor;
out vec3 vertNormal;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vertColor = diffuseColor;
    vertNormal = normalize(vec3(model * vec4(normal, 0.0f)).xyz);
}
