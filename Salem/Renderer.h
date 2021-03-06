#pragma once
#include <string>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "Camera.h"

class Model;
class ShaderManager;
class InstanceManager;
class TextureManager;

class Renderer
{
public:
	Renderer();
	~Renderer();

	Model* GetModel(std::string path);
	GLuint GetShader(std::string name);
	void GetProjection(glm::mat4 &perspective, glm::mat4 &view);
	
	ShaderManager * GetShaderManager();
	InstanceManager * GetInstanceManager();
	TextureManager* GetTextureManager();
	Camera camera;

private:

	struct impl;
	impl* pImpl;
};

