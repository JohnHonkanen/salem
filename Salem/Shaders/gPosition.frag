// textured.frag
#version 330

layout (location = 0) out vec4 gPosition; 

in vec3 FragPos;

uniform sampler2D gColorMap; 
 
void main(void) {

	gPosition = vec4(FragPos, 1.0);  
}