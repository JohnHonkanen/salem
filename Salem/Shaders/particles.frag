#version 330 core

uniform sampler2D texture0;
out vec4 out_Color;
in vec3 out_pColor;
void main(void) {
	
	out_Color = texture(texture0, gl_PointCoord) + vec4(out_pColor, 0.0);
}

