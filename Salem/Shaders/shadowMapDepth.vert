#version 330 core
layout (location = 0) in vec3 in_Position;

uniform mat4 model;
uniform lightSpaceMatrix;

void main()
{
	// Take a per-object model, a vertex and transform all vertices to light space using lightSpaceMatrix
    gl_Position = lightSpaceMatrix * model * vec4(in_Position, 1.0);
}