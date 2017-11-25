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

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 out_UV;

//out vec4 out_Color;
layout (location = 0) out vec4 out_Color;
layout (location = 1) out vec4 out_BrightColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmission;
 
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform vec3 viewPosi; 

// Function prototypes
vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);
vec3 calcSpotLight(SpotLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess);

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
	

	// Phase 2: Calculate Spot Light
	result += calcSpotLight(spotLight, Normal, FragPos, viewDir, Diffuse, Specular, Shininess);
	
	// Phase 3: Apply Emission / Glow
	result += Emission;

	// Phase 4: Apply Gamma Correction
	//float gammaValue = 1 / 2.2f;

	//result += pow(result, vec3(gammaValue));

	// Phase 4: Output results
	out_Color = vec4(result , 1.0f);

	// Phase 5: Check for brightness
	// check whether fragment output is higher than threshold, if so output as brightness color
	// calculate brightness of a fragment by properly transforming it to grayscale first (by taking the dot product of both
	// vectors we effectively multiply each individual component of both vectors and add the results together).

	float brightness = dot(out_Color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	
	// If the color exceeds a certain threshold, we output the color to the second colorbuffer that holds all bright regions.
	if(brightness > 1.0f){
		out_BrightColor = vec4(out_Color.rgb, 1.0f);
	} else {
		out_BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
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
	float spec = pow(max(dot(Normal, halfwayDir), 0.0f), Shininess);

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

vec3 calcSpotLight(SpotLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess) {

	// Normalize the resulting direction vector
	vec3 lightDir = normalize(light.position.xyz - FragPos.xyz);

	// Get the halfway vector based on the Blinn-Phong shading model 
	vec3 halfwayDir = normalize(lightDir + viewDir);

	// Diffuse
	float diff = max(dot(Normal, lightDir), 0.0f); // <--- Use Max to avoid dot product going negative when vector is greater than 90 degrees.

	// Specular
	// Blinn-Phong specular shading 
	float spec = pow(max(dot(Normal, halfwayDir), 0.0f), Shininess);
	
	// Attenuation
	float Distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * Distance + light.quadratic * (Distance * Distance));

	// Spotlight Intensity (and to apply soft edges) <- We use : Intesity = (theta(in degrees) -  outerCutoff(in degrees)) / epsilon. Where epsilon is the difference between the inner cutOff and the outerCutOff in degrees: epsion = innerCutOff - outerCutOff.  
	// Basically interpolating between the outer cosine and the inner cosine based on the ? value
	float theta = dot(lightDir, normalize(-light.direction)); // We negative light so that the vector is pointing towards the light direction, not from.
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); // We clamp the first argument between the values 0.0 and 1.0 to make sure that the intensity values don't end up outside the [0, 1] interval.

	// Combine results
	vec3 ambient = light.ambient * Diffuse;
	vec3 diffuse = light.diffuse * diff * Diffuse;
	vec3 specular = light.specular * spec * Specular;
	
	ambient *= attenuation * intensity; // If prefered. Remove ambient attenuation to ensure that light in spotlight isn't too dark when we move too far away.
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}
