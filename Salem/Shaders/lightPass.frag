// textured.frag
#version 330

struct PointLight {
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 out_Color;

in vec2 out_UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
 
uniform float shininess;

// Function prototypes
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main(void) {
	
	// Retrieve information from G-Buffer
	vec3 FragPos = texture(gPosition, out_UV).rgb;
	vec3 Normal = texture(gNormal, out_UV).rgb;
	vec4 Albedo = texture(gAlbedoSpec, out_UV);


	// Phase 1: Calculate Point Light
	//vec3 result = calcPointLight(pointLight, normal, FragPos, viewDir);

	// Phase 2: Apply Gamma Correction
	//float gammaValue = 1 / 2.2f;

	//result += pow(result, vec3(gammaValue));


	// Phase 3: Output results
	out_Color = vec4(Normal, 1.0f); // vec4(result, 1.0f);
}