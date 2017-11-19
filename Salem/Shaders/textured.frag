#version 330 core

in vec2 out_UV;

out vec4 out_Color;

uniform sampler2D texture0;

void main(void) {
	
	vec3 finalTexture = texture(texture0, out_UV).rgb;

	out_Color = vec4(finalTexture, 1.0f);
}

