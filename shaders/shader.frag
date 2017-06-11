#version 330 core

// render to two different color buffers
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

const int NUM_CASCADES = 3;

in vec3 vertNormal;
in vec3 vertFragPosition;
in vec4 vertFragPositionsLightSpace[NUM_CASCADES];
in float vertClipSpaceZPosition;

uniform vec3 cameraPosition;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossyness;
uniform sampler2D shadowMaps[NUM_CASCADES];
uniform float cascadeEndsClipSpace[NUM_CASCADES];

void main() {
    vec3 light = normalize(vec3(-3.5f, -5.3f, 7.0f));   // TODO: unhardcode this
    float normalDotLight = dot(vertNormal, light);

    int cascadeIndex = 0;
    for (; cascadeIndex < NUM_CASCADES; cascadeIndex++) {
        if (vertClipSpaceZPosition <= cascadeEndsClipSpace[cascadeIndex]) {
            break;
        }
    }

    // variance shadow mapping (VSM), reference: http://www.punkuser.net/vsm/vsm_paper.pdf
    vec4 shadowMapCoords = vertFragPositionsLightSpace[cascadeIndex] / vertFragPositionsLightSpace[cascadeIndex].w;   // perspective divide
    float currentDepth = shadowMapCoords.z;

    // Chebychev's inequality
    vec2 moments = texture2D(shadowMaps[cascadeIndex], shadowMapCoords.xy).rg;
    float p = 1.0f;
    if (currentDepth > moments.x) {
        float variance = moments.y - moments.x * moments.x;     // Algebraic formula for the variance (Verschiebungssatz)
        variance = max(variance, 0.002);
        float currentDepthMinusFirstMoment = currentDepth - moments.x;
        p = variance / (variance + currentDepthMinusFirstMoment * currentDepthMinusFirstMoment);
    }

    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    vec3 ambient = vec3(0.1f);

    vec3 diffuse = max(normalDotLight, 0.0f) * diffuseColor * max(p, 0.4f);

    vec3 halfVector = normalize(light + cameraVector);
    vec3 glossy = pow(max(dot(vertNormal, halfVector), 0.0f), glossyness) * specularColor * p;

    outColor = vec4(ambient + diffuse * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.7f, 1.0f);
}
