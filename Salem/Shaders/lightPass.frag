// textured.frag
#version 330

out vec4 out_Color;

in vec2 out_UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
 
void main(void) {
	
	// Retrieve information from G-Buffer
	vec3 FragPos = texture(gPosition, out_UV).rgb;
	vec3 Normal = texture(gNormal, out_UV).rgb;
	vec3 Albedo = texture(gAlbedoSpec, out_UV).rgb;

	out_Color = vec4(Albedo, 1.0f);
}