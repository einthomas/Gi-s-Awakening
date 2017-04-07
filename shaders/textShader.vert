#version 330 core

layout (location = 0) in vec4 vertex;	// contains the position and texture coords

uniform mat4 projection;

out vec2 texCoords;

void main() {
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	texCoords = vertex.zw;
}
