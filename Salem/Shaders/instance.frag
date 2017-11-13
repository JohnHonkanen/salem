// textured.frag
#version 330

layout (location = 0) out vec3 gPosition; 
layout (location = 1) out vec3 DiffuseOut; 
layout (location = 2) out vec3 gNormal; 
layout (location = 3) out vec3 gAlbedoSpec; 

in vec2 UV;
in vec3 Normal;  
in vec3 FragPos;

uniform sampler2D gColorMap; 
 
void main(void) {

	vec3 result = vec3(0.0, 1.0, 0.3);

	gPosition = FragPos; 
    DiffuseOut = vec4(result, 1.0).xyz; 
    gNormal = normalize(Normal); 
    gAlbedoSpec = vec3(UV, 0.0);
}