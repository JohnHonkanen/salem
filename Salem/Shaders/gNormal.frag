// textured.frag
#version 330

layout (location = 0) out vec4 gNormal; 

in vec3 Normal;

uniform sampler2D gColorMap; 
 
void main(void) {

	gNormal = vec4(Normal, 1.0);  
}