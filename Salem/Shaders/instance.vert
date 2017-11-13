// textured.vert
// use textures, but no lighting
#version 330

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;
layout (location = 4) in mat4 aInstanceMatrix; // Model Matrix of instance

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // Vertex Position 
out vec3 ex_Normal;
out vec2 ex_UV;

//in  vec3 in_Position;


// multiply each vertex position by the MVP matrix
void main(void) {

	vec3 FragPos = vec3(aInstanceMatrix * vec4(in_Position, 1.0));
	ex_UV = in_UV;
	ex_Normal = in_Normal; 
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}