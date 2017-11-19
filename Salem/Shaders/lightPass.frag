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
uniform sampler2D gEmission;
 
uniform PointLight pointLight;
uniform vec3 viewPosi; 



// Function prototypes
vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);

void main(void) {
	
	// Retrieve information from G-Buffer
	vec3 FragPos = texture(gPosition, out_UV).rgb;
	vec3 Normal = texture(gNormal, out_UV).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, out_UV).rgb;
	float Specular = texture(gAlbedoSpec, out_UV).a;
	vec3 Emission = texture(gEmission, out_UV).rgb;
	float Shininess = texture(gEmission, out_UV).a;

	// Properties:
	vec3 viewDir = normalize(viewPosi - FragPos);

	// Phase 1: Calculate Point Light
	vec3 result = calcPointLight(pointLight, Normal, FragPos, viewDir, Diffuse, Specular, Shininess);

	// Phase 2: Apply Emission / Glow
	result += Emission;

	// Phase 3: Apply Gamma Correction
	float gammaValue = 1 / 2.2f;

	result += pow(result, vec3(gammaValue));

	// Phase 4: Output results
	out_Color = vec4(result , 1.0f);
}


vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess){

	// Normalize the resulting direction vector
	vec3 lightDir = normalize(light.position.xyz - FragPos.xyz);

	// Get the halfway vector based on the Blinn-Phong shading model 
	vec3 halfwayDir = normalize(lightDir + viewDir);

	// Diffuse
	float diff = max(dot(Normal, lightDir), 0.0f); // <--- Use max to  avoid dot product going negative when greater than 90 degree's.

	// Specular
	// Blinn-Phong specular shading 
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess);

	// Attenuation
	float Distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * Distance + light.quadratic * (Distance * Distance));

	// Combine results
	vec3 ambient = light.ambient * Diffuse;
	vec3 diffuse = light.diffuse * diff *  Diffuse;
	vec3 specular = light.specular * spec * Specular;

	//Debug
	//diffuse = vec3(diff, Diffuse.y, Diffuse.z);

	//return diffuse;

	//end of Debug

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}