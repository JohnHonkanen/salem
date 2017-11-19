#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;
layout(location = 5) in vec3 in_Tangent;
layout(location = 6) in vec3 in_Bitangent;
layout(location = 7) in mat4 ImodelMatrix;
layout(location = 11) in ivec4 in_BoneIds;
layout(location = 12) in vec4 in_Weights;

uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

out vec3 FragPos;
//out vec3 out_Normal;
out vec2 out_UV;
out mat3 out_TBN;

// multiply each vertex position by the MVP matrix
void main(void) {

	float totalWeights = in_Weights[0] + in_Weights[1] + in_Weights[2] + in_Weights[3];
	vec4 weightPerc = in_Weights * (1/totalWeights);

	mat4 boneTransform = bones[in_BoneIds[0]] * weightPerc[0];
	boneTransform += bones[in_BoneIds[1]] * weightPerc[1];
	boneTransform += bones[in_BoneIds[2]] * weightPerc[2];
	boneTransform += bones[in_BoneIds[3]] * weightPerc[3];
	
	out_UV = in_UV;

	// Calculate Vertex Positions
	FragPos = vec3(ImodelMatrix * boneTransform * vec4(in_Position, 1.0));
	
	//out_Normal = mat3(transpose(inverse(model))) * in_Normal;

	//mat3 normalMatrix = transpose(inverse(mat3(model)));

    // Gram/Schmidt process to orthogolize the TBN vector so that each vector is again perpendicular to the other vectors.

	vec3 T = normalize(vec3(ImodelMatrix * vec4(in_Tangent, 0.0f)));
	vec3 N = normalize(vec3(ImodelMatrix * boneTransform * vec4(in_Normal, 0.0f)));

	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);

	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T); // normalize(vec3(normalMatrix * in_Bitangent));  

	out_TBN = transpose(mat3(T, B, N));

	gl_Position = projection * view * vec4(FragPos, 1.0f);
	
}