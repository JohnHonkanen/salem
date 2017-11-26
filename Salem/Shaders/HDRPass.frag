#version 330 core

in vec2 out_UV;

out vec4 out_Color;

uniform sampler2D HDR; // <----- HDR Buffer
//uniform float exposure;
uniform sampler2D bloomBlur;

void main(void) {
	
	const float gamma = 2.2f;

	vec3 hdrColor = texture(HDR, out_UV).rgb;

	// Phase 1: Apply HDR Tone Mapping (+ Bloom Color)
	vec3 HDRBufferColor = texture(HDR, out_UV).rgb; 
	//vec3 bloomColor = texture(bloomBlur, out_UV).rgb;
	//HDRBufferColor += bloomColor; 

	// Reinhard tone mapping
	vec3 result = HDRBufferColor / (HDRBufferColor + vec3(1.0f));

	// Exposure tone mapping
	//vec3 result = vec3(1.0f) - exp(-HDRBufferColor * exposure);

	// Phase 2: Apply Gamma Correction
	float gammaValue = 1.0f / gamma;

	result = pow(result, vec3(gammaValue));
	
	//out_Color = vec4(1.0f, 0.0f, 0.0f , 1.0f);
	out_Color = vec4(HDRBufferColor, 1.0f);
}

