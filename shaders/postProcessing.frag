#version 330 core

in vec2 vertTextureCoords;

uniform sampler2D mainImage;
uniform sampler2D brightSpotsBloomImage;
uniform sampler2D hudTexture;
out vec4 outColor;

void main() {
    vec3 color = texture(hudTexture, vertTextureCoords).rgb;
	color += (texture(mainImage, vertTextureCoords).rgb + texture(brightSpotsBloomImage, vertTextureCoords).rgb) * (vec3(1.0) - color);

    //float depthValue = texture(mainImage, vertTextureCoords).r;
    
    // gamma-correct
    color = pow(color, vec3(1.0f / 2.2f));

    outColor = vec4(vec3(color), 1.0f);
}
