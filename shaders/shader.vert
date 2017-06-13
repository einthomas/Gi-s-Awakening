#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

const int NUM_CASCADES = 3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[NUM_CASCADES];

out vec3 vertNormal;
out vec3 vertFragPosition;
out vec4 vertFragPositionsLightSpace[NUM_CASCADES];
out float vertClipSpaceZPosition;
out vec2 vertUV;

void main() {
    vec4 worldPosition = model * vec4(position, 1.0f);
    gl_Position = projection * view * worldPosition;
    vertNormal = normalize(vec3(model * vec4(normal, 0.0f)).xyz);
    vertFragPosition = vec3(worldPosition);
    for (int i = 0; i < NUM_CASCADES; i++) {
        vertFragPositionsLightSpace[i] = lightSpaceMatrices[i] * worldPosition;
    }
    vertClipSpaceZPosition = gl_Position.z;
    vertUV = uv;
}
