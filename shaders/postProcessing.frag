#version 330 core

in vec2 vertTextureCoords;

uniform sampler2D mainImage;
uniform sampler2D brightSpotsBloomImage;
out vec4 outColor;

void main() {
    vec3 color = texture(mainImage, vertTextureCoords).rgb + texture(brightSpotsBloomImage, vertTextureCoords).rgb;
    
    // gamma-correct
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(color, 1.0f);
}
