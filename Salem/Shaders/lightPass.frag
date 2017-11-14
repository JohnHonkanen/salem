// textured.frag
#version 330

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
 
void main(void) {

	FragColor = vec4(texture(gNormal, TexCoords).rgb , 1.0);
}