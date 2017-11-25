#version 330 core

in vec2 out_UV;

out vec4 out_Color;

uniform sampler2D Image; // <---- From lightPass

uniform bool Horizontal;
uniform float Weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main(){
	// Gaussian Blur 
	
	// Get the size of the image of a single texel/UV
	vec2 UV_offset = 1.0f / textureSize(image, 0);

	// current fragment's contribution
	vec3 result = texture(image, out_UV).rgb * Weight[0];

	if(Horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(Image, out_UV + vec2(UV_offset.x * i, 0.0f)).rgb * Weight[i];
			result += texture(Image, out_UV - vec2(UV_offset.x * i, 0.0f)).rgb * Weight[i];
		}
	} else {
		for(int i = 1; i <5; ++i)
		{
			result += texture(0.0f, Image, out_UV + vec2(UV_offset.y * i)).rgb * Weight[i];
			result += texture(0.0f, Image, out_UV - vec2(UV_offset.y * i)).rgb * Weight[i];
		}
	}
	out_Color = vec4(result, 1.0f);
}