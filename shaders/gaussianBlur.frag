#version 330 core

in vec2 vertTextureCoords;

uniform bool horizontalBlur;
uniform sampler2D image;
uniform int imageWidth;
uniform int imageHeight;
uniform float offset[3] = float[] (0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[] (0.2270270270, 0.3162162162, 0.0702702703);

out vec4 outColor;

void main(void) {
    vec3 color;

    int imageDimension = horizontalBlur ? imageWidth : imageHeight;
    color = texture(image, vertTextureCoords).rgb * weight[0];

    for (int i = 1; i < 3; i++) {
        vec2 offsetVector = vec2(0.0f);
        if (horizontalBlur) {
            offsetVector.x = offset[i];
        } else {
            offsetVector.y = offset[i];
        }
        color += texture(image, (vertTextureCoords + offsetVector / float(imageDimension))).rgb * weight[i];
        color += texture(image, (vertTextureCoords - offsetVector / float(imageDimension))).rgb * weight[i];
    }

    outColor = vec4(color, 1.0f);
}
