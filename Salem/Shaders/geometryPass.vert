// textured.vert
// use textures, but no lighting
#version 330

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;
layout (location = 7) in mat4 ImodelMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

// multiply each vertex position by the MVP matrix
void main(void) {

	vec3 FragPos = vec3(aInstanceMatrix * vec4(position, 1.0));
	UV = uv;
	Normal = normal;//(transpose(inverse(mat4(aInstanceMatrix))) * vec4(normal, 0.0)).xyz;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}