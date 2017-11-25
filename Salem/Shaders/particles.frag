#version 330 core

uniform sampler2D texture0;
out vec4 out_Color;

void main(void) {
	
	out_Color = texture(texture0, gl_PointCoord);
}

