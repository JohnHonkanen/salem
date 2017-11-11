#pragma once
#include <string>
#include <GL\glew.h>
#include <glm\glm.hpp>

class Model;

class Renderer
{
public:
	Renderer();
	~Renderer();

	Model* GetModel(std::string path);
	GLuint GetShader(std::string name);
	void GetProjection(glm::mat4 &perspective, glm::mat4 &view);

private:

	struct impl;
	impl* pImpl;
};

