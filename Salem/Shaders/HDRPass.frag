#version 330 core

in vec2 out_UV;

out vec4 out_Color;

uniform sampler2D HDR; // <----- HDR Buffer

void main(void) {
	
	const float gamma = 2.2f;

	// Phase 1: Apply HDR Tone Mapping
	vec3 HDRBufferColor = texture(HDR, out_UV).rgb; 

	// Reinhard tone mapping
	vec3 result = HDRBufferColor / (HDRBufferColor + vec3(1.0f));

	// Phase 2: Apply Gamma Correction
	float gammaValue = 1.0f / gamma;

	result = pow(result, vec3(gammaValue));
	

	out_Color = vec4(result, 1.0f);
}

