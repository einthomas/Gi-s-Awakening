#version 330 core

// render to two different color buffers
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

const int NUM_CASCADES = 3;

in vec3 vertNormal;
in vec3 vertFragPosition;
in vec4 vertFragPositionsLightSpace[NUM_CASCADES];
in float vertClipSpaceZPosition;
in vec2 vertUV;

uniform vec3 cameraPosition;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossyness;

uniform bool shadowsActivated = true;
uniform sampler2D shadowMaps[NUM_CASCADES];
uniform float cascadeEndsClipSpace[NUM_CASCADES];

uniform sampler2D lightMap;
uniform float lightMapScale;
uniform vec2 lightMapPosition;

void main() {
    vec3 lightDirection = normalize(vec3(-3.5f, -5.3f, 7.0f));   // TODO: unhardcode this
    vec3 light = vec3(1.0, 0.768, 0.216);
    float normalDotLight = dot(vertNormal, lightDirection);
    
    float p = 1.0f;
    if (shadowsActivated) {
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
        if (currentDepth > moments.x) {
            float variance = moments.y - moments.x * moments.x;     // Algebraic formula for the variance (Verschiebungssatz)
            variance = max(variance, 0.002);
            float currentDepthMinusFirstMoment = currentDepth - moments.x;
            p = variance / (variance + currentDepthMinusFirstMoment * currentDepthMinusFirstMoment);
        }
    }

    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    // ambient
    vec3 ambient = pow(texture(
        lightMap,
        vec2(0, 1) + (lightMapPosition + vertUV * lightMapScale) * vec2(1, -1)
    ).rgb, vec3(2.2));

    // diffuse
    vec3 diffuse = max(normalDotLight, 0.0f) * diffuseColor * max(p, 0.4f) * light;

    // specular
    vec3 halfVector = normalize(lightDirection + cameraVector);
    vec3 glossy = pow(
        max(dot(vertNormal, halfVector), 0.0f), glossyness
    ) * specularColor * p * light;

    outColor = vec4(ambient + diffuse * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.5f, 1.0f);
}
