#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 7) in mat4 ImodelMatrix; // instance model matrix

uniform mat4 lightSpaceMatrix;

void main()
{
	// Take a per-object model, a vertex and transform all vertices to light space using lightSpaceMatrix
    gl_Position = lightSpaceMatrix * ImodelMatrix * vec4(in_Position, 1.0);
}