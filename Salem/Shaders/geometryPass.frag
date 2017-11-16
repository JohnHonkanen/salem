#version 330 core

layout (location = 0) out vec3 gPosition; 
layout (location = 1) out vec3 gNormal; 
layout (location = 2) out vec4 gAlbedoSpec; 
//layout (location = 3) out vec4 gEmission;

in vec3 FragPos;
in vec3 out_Normal;  
in vec2 out_UV;
in mat3 out_TBN;


uniform vec3 viewPosi;
uniform vec3 lightPosi;

uniform sampler2D diffuseMap; 
uniform sampler2D specularMap;
uniform sampler2D normalMap;
//uniform sampler2D emissionMap;
 
void main(void) {

	gPosition = FragPos;  
    //gNormal = out_Normal; 

	// Store diffuse per-fragment color.
    gAlbedoSpec.rgb = texture(diffuseMap, out_UV).rgb;

	// Store specular intesity in AlbedoMap's alpha component
	gAlbedoSpec.a = texture(specularMap, out_UV).r;

	// We obtain the normal from the normal map in a range [0, 1]
	// Then tranform normal vector to range [-1 , 1]. Note: This normal is in the tangent space.
	vec3 normal = normalize(texture(normalMap, out_UV).rgb * 2.0f - 1.0f);

	// Store the per-fragment normals into the gbuffer
	gNormal = normalize(out_TBN * normal); 
	
	// Tangent Space Calcs
	



	//gEmission.rgb = texture(emissionMap, out_UV).rgb;
	//gEmission.a = shininess;
}