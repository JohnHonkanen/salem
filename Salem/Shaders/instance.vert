#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;
layout(location = 4) in mat4 ImodelMatrix; // Instance Model Matrix
layout(location = 9) in vec3 in_Tangent;
layout(location = 10) in vec3 in_Bitangent;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPosi;
uniform vec3 lightPosi;

out vec3 tangentFragPos; // Vertex Position 
out vec3 tangentViewPos;
out vec3 tangentLightPos;
out vec2 ex_UV;

// multiply each vertex position by the MVP matrix
void main(void) {

	gl_Position = projection * view * ImodelMatrix * vec4(in_Position, 1.0f);

	ex_UV = in_UV;
	
	// Calculate Vertex Positions
	vec3 FragPos = vec3(ImodelMatrix * vec4(in_Position, 1.0f));
	
	// Calculate World Normals <----- Ideally should be done in CPU instead, less expensive. 
	//vec3 ex_Normal = mat3(transpose(inverse(ImodelMatrix))) * in_Normal; 
    
	// Gram/Schmidt process to orthogolize the TBN vector so that each vector is again perpendicular to the other vectors.
	//mat3 normalMatrix = mat3(transpose(inverse(ImodelMatrix)));

	vec3 T = normalize(vec3(ImodelMatrix * vec4(in_Tangent, 0.0f)));
	vec3 N = normalize(vec3(ImodelMatrix * vec4(in_Normal, 0.0f)));

	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);

	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);

	tangentLightPos = TBN * lightPosi;
	tangentViewPos = TBN * viewPosi;
	tangentFragPos = TBN * FragPos;
	
}