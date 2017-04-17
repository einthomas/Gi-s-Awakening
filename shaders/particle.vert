#version 330 core

layout (location = 0) in vec3 vertices;
layout (location = 1) in vec3 position;
layout (location = 2) in vec4 color;

uniform mat4 view;
uniform mat4 projection;

out vec4 vertColor;

void main() {
	vec3 particleWorldSpacePosition = position.xyz;
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);
    vec3 vertexWorldSpacePosition = particleWorldSpacePosition + cameraRight * vertices.x * 0.01f + cameraUp * vertices.y * 0.01f;
    gl_Position = projection * view * vec4(vertexWorldSpacePosition, 1.0f);
    vertColor = color;
}
