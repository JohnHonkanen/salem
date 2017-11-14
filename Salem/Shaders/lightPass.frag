// textured.frag
#version 330

out vec4 out_Color;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
 
void main(void) {

	out_Color = vec4(texture(gPosition, TexCoords).rgb , 1.0);
}