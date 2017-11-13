// textured.frag
#version 330

out vec4 FragColor;

in vec2 UV;

in vec3 Normal;  
in vec3 FragPos;

 
void main(void) {

	vec3 result = vec3(0.0, 1.0, 0.3);

    FragColor = vec4(result, 1.0); 
}