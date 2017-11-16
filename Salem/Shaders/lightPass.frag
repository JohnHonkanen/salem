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
 
uniform vec3 viewPosi; 
uniform vec3 lightPos;
uniform float shininess;
uniform PointLight pointLight;

// Function prototypes
vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec4 Albedo);

void main(void) {
	
	// Retrieve information from G-Buffer
	vec3 FragPos = texture(gPosition, out_UV).rgb;
	vec3 Normal = texture(gNormal, out_UV).rgb;
	vec4 Albedo = texture(gAlbedoSpec, out_UV);

	// Properties:
	vec3 viewDir = normalize(viewPosi - FragPos);

	// Phase 1: Calculate Point Light
	vec3 result = calcPointLight(pointLight, Normal, FragPos, viewDir, Albedo);

	// Phase 2: Apply Gamma Correction
	//float gammaValue = 1 / 2.2f;

	//result += pow(result, vec3(gammaValue));


	// Phase 3: Output results
	out_Color = vec4(result, 1.0f);
}


vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec4 Albedo){

	// Normalize the resulting direction vector
	vec3 lightDir = normalize(light.position.xyz - FragPos.xyz);

	// Get the halfway vector based on the Blinn-Phong shading model 
	vec3 halfwayDir = normalize(lightDir + viewDir);

	// Diffuse
	float diff = max(dot(Normal, lightDir), 0.0f); // <--- Use max to  avoid dot product going negative when greater than 90 degree's.

	// Specular
	// Blinn-Phong specular shading 
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);

	// Attenuation
	float Distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * Distance + light.quadratic * (Distance * Distance));

	// Combine results
	vec3 ambient = light.ambient * Albedo.rgb;
	vec3 diffuse = light.diffuse * diff * Albedo.rgb;
	vec3 specular = light.specular * spec * Albedo.a;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return Albedo.rgb; // (ambient + diffuse + specular);
}