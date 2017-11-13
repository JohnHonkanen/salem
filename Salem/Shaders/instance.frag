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
in vec3 ex_Normal;  
in vec3 FragPos; // Vertex Position

// Uniforms
uniform PointLight pointLight;
uniform Material material;
uniform vec3 viewPos;
uniform vec3 diffuse;
uniform vec3 specular;


out vec4 out_Color; 
 
 // Function prototypes
 vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);


void main(void) {

	// Properties
	vec3 normal = normalize(ex_Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	// Phase 1: Calculate Point Light
	vec3 result = calcPointLight(pointLight, normal, FragPos, viewDir);

	// Phase 2: Output results
	result += vec3(0.0, 1.0, 0.3);

    out_Color = vec4(result, 1.0); 
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir){

	// Normalize the resulting direction vector
	vec3 lightDir = normalize(light.position.xyz - FragPos.xyz);

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0f); // <--- Use max to  avoid dot product going negative when greater than 90 degree's.

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal); // <---- calculate the reflection of the light
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	spec = max(dot(viewDir, reflectDir), 0.0);

	// Attenuation
	//float Distance = length(light.position - FragPos);
	//float attenuation = 1.0f / (light.constant + light.linear * Distance + light.quadratic * (Distance * Distance));

	// Combine results
	vec3 ambient = light.ambient; //* vec3(texture(diffuse, ex_UV));
	vec3 diffuse = light.diffuse * diff; // * vec3(texture(diffuse, ex_UV));
	vec3 specular = light.specular * spec; // * vec3(texture(specular, ex_UV));

	return (ambient + diffuse + specular);
}