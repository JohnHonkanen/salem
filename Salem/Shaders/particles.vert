#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_pColor;

void main()
{
    out_pColor = in_Color;
	gl_Position = projection * view * model * vec4(in_Position, 1.0);
}