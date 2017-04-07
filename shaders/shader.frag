#version 330 core

// render to two different color buffers
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

in vec3 vertColor;
in vec3 vertNormal;
in vec3 vertCameraVector;

void main() {
    // TODO: unhardcode this
    vec3 light = normalize(vec3(-0.5f, -0.3f, 1.0f));

    float diffuse = max(dot(vertNormal, light), 0.0f);

    vec3 halfVector = normalize(light + vertCameraVector);
    float glossy = pow(max(dot(vertNormal, halfVector), 0.0f), 100);

    vec3 color = vec3((diffuse + glossy) * 0.75);
    outColor = vec4(color, 1.0f);
    if (color.r > 0.5f && color.g > 0.5f && color.b > 0.5f) {
        brightSpotColor = vec4(color * 0.5f, 1.0f);
    } else {
        brightSpotColor = vec4(0.0f);
    }

    // gamma-correct
    //color = pow(color, vec3(1.0f / 2.2f));
}
