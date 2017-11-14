#version 330 core

struct Material {

float shininess;
};

struct PointLight {
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 ex_UV;
//in vec3 ex_Normal;  
in vec3 FragPos; // Vertex Position
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

// Uniforms
uniform PointLight pointLight;
uniform Material material;
uniform vec3 viewPos;
uniform vec3 diffuse;
uniform vec3 specular;
uniform bool normalMapping; // Check if normal mapping

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D emissionMap;
uniform sampler2D normalMap;

out vec4 out_Color; 
 
// Function prototypes
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main(void) {

	// Properties
	// We obtain the normal from the normal map in a range [0, 1]
	vec3 normal = texture(normalMap, ex_UV).rgb;
	// Then tranform normal vector to range [-1 , 1]. Note: This normal is in the tangent space.
	normal = normalize(normal * 2.0 - 1.0);
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

	// Phase 1: Calculate Point Light
	vec3 result = calcPointLight(pointLight, normal, FragPos, viewDir);

	// Phase 2: Apply Gamma Correction
	float gammaValue = 1 / 2.2f;

	result += pow(result, vec3(gammaValue));


	// Phase 3: Output results
    out_Color = vec4(result, 1.0); 
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir){

	// Normalize the resulting direction vector
	vec3 lightDir = normalize(light.position.xyz - FragPos.xyz);

	// Get the halfway vector based on the Blinn-Phong shading model 
	vec3 halfwayDir = normalize(lightDir + viewDir);

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0f); // <--- Use max to  avoid dot product going negative when greater than 90 degree's.

	// Specular
	
	// Old depricated specular calculations
	//vec3 reflectDir = reflect(-lightDir, normal); // <---- calculate the reflection of the light
	//spec = max(dot(viewDir, reflectDir), 0.0);

	// Blinn-Phong specular shading model
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	// Attenuation
	float Distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * Distance + light.quadratic * (Distance * Distance));

	// Combine results
	vec3 ambient = light.ambient * vec3(texture(diffuseMap, ex_UV));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, ex_UV));
	vec3 specular = light.specular * spec * vec3(texture(specularMap, ex_UV));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}