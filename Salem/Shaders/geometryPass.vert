#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;
layout(location = 5) in vec3 in_Tangent;
layout(location = 6) in vec3 in_Bitangent;
layout (location = 7) in mat4 ImodelMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 out_Normal;
out vec2 out_UV;

// multiply each vertex position by the MVP matrix
void main(void) {

	gl_Position = projection * view * ImodelMatrix * vec4(in_Position, 1.0f);

	out_UV = in_UV;

	// Calculate Vertex Positions
	vec3 FragPos = vec3(ImodelMatrix * vec4(in_Position, 1.0));
	
	out_Normal = in_Normal;     //(transpose(inverse(mat4(ImodelMatrix))) * vec4(in_Normal, 0.0)).xyz;

    //gl_Position = projection * view * vec4(FragPos, 1.0);
}