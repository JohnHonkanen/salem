// textured.vert
// use textures, but no lighting
#version 330

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;
layout (location = 7) in mat4 aInstanceMatrix;

uniform mat4 view;
uniform mat4 projection;


// multiply each vertex position by the MVP matrix
void main(void) {
	vec3 FragPos = vec3(aInstanceMatrix * vec4(position, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}