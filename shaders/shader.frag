#version 330 core

in vec3 vertColor;
in vec3 vertNormal;
in vec3 vertCameraVector;

out vec4 outColor;

void main() {
    // TODO: unhardcode this
    vec3 light = normalize(vec3(-0.5f, -0.3f, 1.0f));

    float difuse = max(dot(vertNormal, light), 0.0f);

    vec3 halfVector = normalize(light + vertCameraVector);
    float glossy = pow(max(dot(vertNormal, halfVector), 0.0f), 100);

    vec3 color = vec3((difuse + glossy) * 0.75);

    // gamma-correct
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}
