#version 330 core

in vec2 vertTextureCoords;

uniform bool horizontalBlur;
uniform sampler2D image;
uniform int imageWidth;
uniform int imageHeight;
const int KERNEL_SIZE = 4;
uniform float offset[KERNEL_SIZE] = float[] (0.0, 1.3846154, 3.2307692, 5.076923);
uniform float weight[KERNEL_SIZE] = float[] (0.22558594, 0.31420898, 0.06982422, 0.0031738281);

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
