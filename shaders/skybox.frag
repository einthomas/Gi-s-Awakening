#version 330 core

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

uniform samplerCube skybox;

in vec3 vertTextureCoords;

void main() {
    outColor = vec4(texture(skybox, vertTextureCoords).rgb, 1.0f);
    brightSpotColor = vec4(0.0f);
}
