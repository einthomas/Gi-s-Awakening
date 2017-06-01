#version 330 core

out vec4 vertPosition;

out vec4 outColor;

void main() {
    float depth = vertPosition.z / vertPosition.w;      // perspective divide
    outColor = vec4(depth, depth * depth, 0.0f, 0.0f);  // output first and second moment
}
