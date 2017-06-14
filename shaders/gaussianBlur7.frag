#version 330 core

in vec2 vertTextureCoords;

uniform bool horizontalBlur;
uniform sampler2D image;
uniform int imageWidth;
uniform int imageHeight;
const int KERNEL_SIZE = 7;
uniform float offset[KERNEL_SIZE] = float[] (0.0, 1.4716982, 3.4339623, 5.3962264, 7.35849, 9.320755, 11.283019);
uniform float weight[KERNEL_SIZE] = float[] (0.11013008, 0.20073974, 0.1384412, 0.07061618, 0.026433865, 0.007174906, 0.001388033);

out vec4 outColor;

void main(void) {
    vec3 color;

    int imageDimension = horizontalBlur ? imageWidth : imageHeight;
    color = texture(image, vertTextureCoords).rgb * weight[0];
    
    vec2 direction = vec2(float(horizontalBlur), float(!horizontalBlur));
    vec2 offsetVector = vec2(0.0f);
    for (int i = 1; i < KERNEL_SIZE; i++) {
        offsetVector = vec2(offset[i]) * direction;
        vec2 offsetVectorByImageDimension = offsetVector / float(imageDimension);
        color += texture(image, (vertTextureCoords + offsetVectorByImageDimension)).rgb * weight[i];
        color += texture(image, (vertTextureCoords - offsetVectorByImageDimension)).rgb * weight[i];
    }
    
    outColor = vec4(color, 1.0f);
}
