// textured.frag
#version 330

layout (location = 0) out vec4 gPosition; 
layout (location = 1) out vec4 gNormal; 
layout (location = 2) out vec4 gAlbedoSpec; 

in vec2 UV;
in vec3 Normal;  
in vec3 FragPos;

uniform vec3 viewPosi;
uniform vec3 lightPosi;

uniform sampler2D diffuseMap; 
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D emissionMap;
 
void main(void) {

	gPosition = vec4(FragPos, 1.0f);  
    gNormal = vec4(Normal, 1.0f); 
    gAlbedoSpec = vec4(0.0f, 1.0f, 3.0f, 1.0f);
}