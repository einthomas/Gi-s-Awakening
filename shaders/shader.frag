#version 330 core

in vec3 vertColor;
in vec3 vertNormal;

out vec4 outColor;

void main() {
    vec3 color = vec3(max(dot(vertNormal, normalize(vec3(-0.5f, -0.3f, 1.0f))), 0.0f));

    // gamma-correct
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}
