#version 330 core

// render to two different color buffers
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightSpotColor;

in vec3 vertNormal;
in vec3 vertFragPosition;
in vec4 vertFragPositionLightSpace;
in vec2 vertUV;

uniform vec3 cameraPosition;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossyness;

uniform sampler2D shadowMap;
uniform sampler2D lightMap;

uniform float lightMapScale;
uniform vec2 lightMapPosition;

void main() {
    // TODO: unhardcode this
    vec3 lightDirection = normalize(vec3(-0.5f, -0.3f, 1.0f));
    vec3 light = vec3(1.0, 0.768, 0.216);

    float normalDotLight = dot(vertNormal, lightDirection);
    // variance shadow mapping (VSM), reference: http://www.punkuser.net/vsm/vsm_paper.pdf
    vec4 shadowMapCoords = vertFragPositionLightSpace / vertFragPositionLightSpace.w;   // perspective divide
    float currentDepth = shadowMapCoords.z;

    // Chebychev's inequality
    vec2 moments = texture2D(shadowMap, shadowMapCoords.xy).rg;
    float p = 1.0f;
    if (currentDepth > moments.x) {
        float variance = moments.y - moments.x * moments.x;     // Algebraic formula for the variance (Verschiebungssatz)
        variance = max(variance, 0.002);
        float currentDepthMinusFirstMoment = currentDepth - moments.x;
        p = variance / (variance + currentDepthMinusFirstMoment * currentDepthMinusFirstMoment);
    }

    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    vec3 ambient = pow(texture(
        lightMap,
        vec2(0, 1) + (lightMapPosition + vertUV * lightMapScale) * vec2(1, -1)
    ).rgb, vec3(2.2));

    vec3 diffuse =
        max(normalDotLight, 0.0f) * diffuseColor * p * light;

    vec3 halfVector = normalize(lightDirection + cameraVector);
    vec3 glossy = pow(
        max(dot(vertNormal, halfVector), 0.0f), glossyness
    ) * specularColor * p * light;

    outColor = vec4((ambient + diffuse) * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.5f, 1.0f);
}
