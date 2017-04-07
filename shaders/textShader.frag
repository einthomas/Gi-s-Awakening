#version 330 core

uniform sampler2D text;
uniform vec3 textColor;

in vec2 texCoords;
out vec4 color;

void main() {
	/*
		sample the color of the bitmap texture. The texture data is
		stored in the red component texture(..., ...).r. By setting
		the alpha value to the data of the red component, the background
		of the character will be transparent
	*/
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);
	color = vec4(textColor, 1.0) * sampled;
}
