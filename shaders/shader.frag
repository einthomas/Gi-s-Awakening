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
uniform sampler2DShadow shadowMap;

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main() {
    vec3 light = normalize(vec3(-3.5f, -5.3f, 7.0f));   // TODO: unhardcode this
    float normalDotLight = dot(vertNormal, light);
    
    float shadowFactor = 0.0f;
    vec2 texelSize = (1.0f / textureSize(shadowMap, 0)) * vertFragPositionLightSpace.w;  // multiply by w because textureProj divides by w
    for (float y = -3.0f; y <= 3.0f; y++) {
        for (float x = -3.0f; x <= 3.0f; x++) {
            shadowFactor += textureProj(shadowMap, vertFragPositionLightSpace + vec4(vec2(x, y) * texelSize, 0.0f, 0.0f));
        }
    }
    shadowFactor /= 32.0f;

    vec3 cameraVector = normalize(cameraPosition - vertFragPosition);

    vec3 ambient = vec3(0.1f);

    vec3 diffuse = max(normalDotLight, 0.0f) * diffuseColor * 0.6f;
    diffuse = diffuse * min(shadowFactor, 0.7f) + diffuse * 0.8f;

    vec3 halfVector = normalize(light + cameraVector);
    vec3 glossy = pow(max(dot(vertNormal, halfVector), 0.0f), glossyness) * specularColor * shadowFactor;

    outColor = vec4(ambient + diffuse * 0.6f, 1.0f);
    brightSpotColor = vec4(glossy * 0.7f, 1.0f);
}
