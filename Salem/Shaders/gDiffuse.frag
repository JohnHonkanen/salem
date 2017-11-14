// textured.frag
#version 330

layout (location = 0) out vec4 gAlbedoSpec; 
 
void main(void) {

	gAlbedoSpec = vec4(0.0, 1.0, 0.3, 1.0);
}