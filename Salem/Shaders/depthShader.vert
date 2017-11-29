#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 7) in mat4 ImodelMatrix;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * ImodelMatrix * model * vec4(in_Position, 1.0);
}