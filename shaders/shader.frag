#version 330 core

in vec3 vertColor;
in vec3 vertNormal;

out vec4 outColor;

void main() {
    vec3 color = vec3(max(dot(vertNormal, vec3(0f, 0f, 1f)), 0f));

    // gamma-correct
    color = pow(color, vec3(1f / 2.2f));

    outColor = vec4(color, 1.0f);
}
