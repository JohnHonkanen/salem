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

vec4 fragPosLightSpace;

//out vec4 out_Color;
layout (location = 0) out vec4 out_Color;
layout (location = 1) out vec4 out_BrightColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmission;
uniform sampler2D depthMap;
 
const int MAX_LIGHTS = 200;
uniform int totalLights;
uniform PointLight pointLight[MAX_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPosi; 

// Shadows
uniform float near_plane;
uniform float far_plane;
uniform mat4 lightSpaceMatrix;

// Function prototypes
vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess, vec4 fragPosLightSpace);
vec3 calcSpotLight(SpotLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess, vec4 fragPosLightSpace);
float calculateShadows(vec4 fragPosLightSpace, vec3 Normal, vec3 FragPos, vec3 lightPos);

void main(void) {

	// Retrieve information from G-Buffer
	vec3 FragPos = texture(gPosition, out_UV).rgb;
	vec3 Normal = texture(gNormal, out_UV).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, out_UV).rgb;
	float Specular = texture(gAlbedoSpec, out_UV).a;
	vec3 Emission = texture(gEmission, out_UV).rgb;
	float Shininess = texture(gEmission, out_UV).a;

	// Retrieve Depthmap information and configure for perspective camera
	float depthValue = texture(depthMap, out_UV).r;

	// Transform the world-space vertex position to light space. 
	fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);

	// Properties:
	vec3 viewDir = normalize(viewPosi - FragPos);

	// Phase 1: Calculate Point Light
	vec3 result;

	int numLight = 0;
	numLight = min(totalLights, MAX_LIGHTS);
	for(int i = 0; i < numLight; i++){
		result += calcPointLight(pointLight[i], Normal, FragPos, viewDir, Diffuse, Specular, Shininess, fragPosLightSpace);
	}

	// Phase 2: Calculate Spot Light
	result += calcSpotLight(spotLight, Normal, FragPos, viewDir, Diffuse, Specular, Shininess, fragPosLightSpace);
	
	// Phase 3: Apply Emission / Glow
	result += Emission;

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
	} 
	else {
		out_BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}


vec3 calcPointLight(PointLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess, vec4 fragPosLightSpace){

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
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec * Specular;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Calculate Shadow
	float shadow = calculateShadows(fragPosLightSpace, Normal, FragPos, light.position.xyz);

	// Combine results
	//return (ambient + diffuse + specular);
	return (ambient + (1.0f - shadow) * (diffuse + specular)) * Diffuse;
}

vec3 calcSpotLight(SpotLight light, vec3 Normal, vec3 FragPos, vec3 viewDir, vec3 Diffuse, float Specular, float Shininess, vec4 fragPosLightSpace) {

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

	// Calculate Shadow
	//float shadow = calculateShadows(fragPosLightSpace, Normal, FragPos, light.position.xyz);

	return (ambient + diffuse + specular);
	//return (ambient +  (1.0f - shadow) * (diffuse + specular)) * Diffuse;
}

float calculateShadows(vec4 fragPosLightSpace, vec3 Normal, vec3 FragPos, vec3 lightPos){
	// perspecitve divide to transform the NDC coordinates to the range of [0,1]
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	// get the closest value from the light's perspective (using the [0,1] range fragPosLight as the coords)
	float closestDepth = texture(depthMap, projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// Calculate the bias ( based on the depth map resolution and slope)
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005f);

	// now check whether current fragPos is in shadow


	// PCF (percentage-closer filtering) to sample the surrounding texels of the depth map and average the results
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	float shadow = 0.0f; 
	vec2 texelSize = 1.0f / textureSize(depthMap, 0);

	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
		}
	}
	// shadow = shadow/9.0f
	shadow /= 9.0f;

	// Force shadow value to 0.0f whenever the projected vector's z coord is larger than 1.0f (when outside the far_plane region of the light's frustrum).
	if(projCoords.z > 1.0f){
		shadow = 0.0f;
	}
	return shadow;

}
