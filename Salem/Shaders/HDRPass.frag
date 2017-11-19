#version 330 core

in vec2 out_UV;

out vec4 out_Color;

uniform sampler2D HDR;

void main(void) {
	
	vec3 hdrColor = texture(HDR, out_UV).rgb;

	out_Color = vec4(hdrColor, 1.0f);
}

