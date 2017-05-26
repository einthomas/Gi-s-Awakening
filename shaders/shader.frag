#version 330 core

// render to two different color buffers
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

in vec3 vertNormal;
in vec3 vertFragPosition;
in vec4 vertFragPositionLightSpace;

uniform vec3 cameraPosition;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossyness;
uniform sampler2D shadowMap;

void main() {
    vec3 shadowMappingCoords = vertFragPositionLightSpace.xyz / vertFragPositionLightSpace.w;
    shadowMappingCoords = shadowMappingCoords * 0.5f + 0.5f;

    float closestDepth = texture(shadowMap, shadowMappingCoords.xy).r;
    float currentDepth = shadowMappingCoords.z - 0.005f;

    vec3 light = normalize(vec3(-0.5f, -0.3f, 1.0f));   // TODO: unhardcode this
    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    vec3 ambient = vec3(0.1f);

    vec3 diffuse = max(dot(vertNormal, light), 0.0f) * diffuseColor * ((closestDepth < currentDepth) ? 0.5f : 1.0f);

    vec3 halfVector = normalize(light + cameraVector);
    vec3 glossy = pow(max(dot(vertNormal, halfVector), 0.0f), glossyness) * specularColor * ((closestDepth < currentDepth) ? 0.0f : 1.0f);

    outColor = vec4(ambient + diffuse * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.7f, 1.0f);

    //outColor = vec4(vec3(texture(shadowMap, shadowMappingUVCoords).r), 1.0f);
}
