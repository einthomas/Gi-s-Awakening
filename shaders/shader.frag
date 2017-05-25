#version 330 core

// render to two different color buffers
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

in vec3 vertNormal;
in vec3 vertFragPosition;

uniform vec3 cameraPosition;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossyness;

void main() {
    // TODO: unhardcode this
    vec3 light = normalize(vec3(-0.5f, -0.3f, 1.0f));
    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    vec3 ambient = vec3(0.1f);

    vec3 diffuse = max(dot(vertNormal, light), 0.0f) * diffuseColor;

    vec3 halfVector = normalize(light + cameraVector);
    vec3 glossy = pow(max(dot(vertNormal, halfVector), 0.0f), glossyness) * specularColor;

    outColor = vec4(ambient + diffuse * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.7f, 1.0f);
}
