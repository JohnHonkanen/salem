// textured.frag
#version 330

layout (location = 0) out vec3 gPosition; 
layout (location = 1) out vec3 gNormal; 
layout (location = 2) out vec4 gAlbedoSpec; 

in vec2 UV;
in vec3 Normal;  
in vec3 FragPos;

uniform sampler2D gDiffuseMap; 
 
void main(void) {

	gPosition = FragPos;  
    gNormal = Normal; 
    gAlbedoSpec = texture(gDiffuseMap, UV);
}