#pragma once
#include <GL\glew.h>
#include <string>

using namespace std;

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	GLuint GetShader(string name);
private:
	struct impl;
	impl* pImpl;

};

