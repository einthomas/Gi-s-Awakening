#version 450 core

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
uniform sampler2DShadow shadowMap;

void main() {
    vec3 light = normalize(vec3(-0.5f, -0.3f, 1.0f));   // TODO: unhardcode this
    float normalDotLight = dot(vertNormal, light);

    //vec3 shadowMappingCoords = vertFragPositionLightSpace.xyz / vertFragPositionLightSpace.w;
    //shadowMappingCoords = shadowMappingCoords * 0.5f + 0.5f;
    //float closestDepth = textureProj(shadowMap, shadowMappingCoords);
    //float currentDepth = shadowMappingCoords.z;
    // calculate a bias to avoid "shadow acne". The larger the angle between normal and light,
    // the larger the bias
    //float bias = max(0.05f * (1.0f - normalDotLight), 0.005f);
    //currentDepth -= bias;
    
    float shadowFactor = textureProj(shadowMap, vertFragPositionLightSpace);
    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    vec3 ambient = vec3(0.1f);

    vec3 diffuse = max(normalDotLight, 0.0f) * diffuseColor * max(shadowFactor, 0.7f);

    vec3 halfVector = normalize(light + cameraVector);
    vec3 glossy = pow(max(dot(vertNormal, halfVector), 0.0f), glossyness) * specularColor * shadowFactor;

    outColor = vec4(ambient + diffuse * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.7f, 1.0f);

    //outColor = vec4(vec3(texture(shadowMap, shadowMappingUVCoords).r), 1.0f);
}
