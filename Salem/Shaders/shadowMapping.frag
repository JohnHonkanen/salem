#version 330 core

in vec2 out_UV;

out vec4 out_Color;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

float LinearDepth(float depth);

void main(void) {
	
	float depthValue = texture(depthMap, out_UV).r;

	vec3 result = vec3(LinearDepth(depthValue) / far_plane); // for perspective

	out_Color = vec4(result, 1.0f);
}

float LinearDepth(float depth){
	float z = depth * 2.0f - 1.0f; 
	
	return (2.0f, near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}