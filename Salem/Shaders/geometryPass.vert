// textured.vert
// use textures, but no lighting
#version 330

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;
layout (location = 4) in mat4 aInstanceMatrix;

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