#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;
layout(location = 4) in mat4 aInstanceMatrix; // Model Matrix of instance
layout(location = 9) in vec3 tangent;
layout(location = 10) in vec3 bitangent;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // Vertex Position 
out vec3 ex_Normal;
out vec2 ex_UV;

// multiply each vertex position by the MVP matrix
void main(void) {

	ex_UV = in_UV;
	
	// Calculate Vertex Positions
	vec3 FragPos = vec3(aInstanceMatrix * vec4(in_Position, 1.0f));
	
	// Calculate World Normals <----- Ideally should be done in CPU instead, less expensive. 
	ex_Normal = mat3(transpose(inverse(aInstanceMatrix))) * in_Normal; 
    
	gl_Position = projection * view * vec4(FragPos, 1.0);
}