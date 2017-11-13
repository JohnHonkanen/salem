#pragma once
#include <GL\glew.h>
#include <string>
#include <glm/glm.hpp>

using namespace std;

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	GLuint GetShader(string name);
	void SetUniformMatrix4fv(GLuint program, const char* location, glm::mat4 value);
	void SetUniform1i(GLuint program, const char* location, float value);
private:
	struct impl;
	impl* pImpl;

};

