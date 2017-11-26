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
	void SetUniformLocation3f(GLuint program, string location, float x, float y, float z);
	void SetUniformLocation1f(GLuint program, string location, float x);
	void SetUniformLocation1i(GLuint program, const char* location, int x);

private:
	struct impl;
	impl* pImpl;

};

