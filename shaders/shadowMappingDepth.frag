#version 330 core

out vec4 vertPosition;

out vec4 outColor;

void main() {
    float depth = vertPosition.z / vertPosition.w;      // perspective divide
    float moment2 = depth * depth;
    float dx = dFdx(depth);
		float dy = dFdy(depth);
		moment2 += 0.25*(dx*dx+dy*dy) ;
    outColor = vec4(depth, moment2, 0.0f, 0.0f);  // output first and second moment
}
