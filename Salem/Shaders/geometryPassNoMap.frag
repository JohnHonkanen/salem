#version 330 core

layout (location = 0) out vec3 gPosition; 
layout (location = 1) out vec3 gNormal; 
layout (location = 2) out vec4 gAlbedoSpec; 
layout (location = 3) out vec4 gEmission;

in vec3 FragPos;
in vec3 out_Normal;  
in vec2 out_UV;
in mat3 out_TBN;


uniform vec3 viewPosi;
uniform vec3 lightPosi;
uniform float shininess;

uniform sampler2D diffuseMap; 
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D emissionMap;
uniform sampler2D hdrBuffer;
 
void main(void) {

	gPosition = FragPos;
    //gNormal = out_Normal; 

	// Store diffuse per-fragment color.
    gAlbedoSpec.rgb = texture(diffuseMap, out_UV).rgb;

	// Store specular intesity in AlbedoMap's alpha component
	gAlbedoSpec.a = texture(specularMap, out_UV).r;

	vec3 normal = out_Normal;

	// Store the per-fragment normals into the gbuffer
	gNormal = normalize(normal); 

	// Deprecated gNormal implementation
	//vec3 normal = texture(normalMap, out_UV).rgb;
	//gNormal = normal; 

	// Emissive/Glow
	gEmission.rgb = texture(emissionMap, out_UV).rgb;
	gEmission.a = shininess;
}