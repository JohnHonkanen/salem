#include "Renderer.h"
#include "InstanceManager.h"
#include "ShaderManager.h"
#include <glm\gtc\matrix_transform.hpp>

struct Renderer::impl {
	InstanceManager instanceManager;
	ShaderManager shaderManager;
	
	glm::mat4 projection;
	glm::mat4 view;

};

Renderer::Renderer()
{
	pImpl = new impl();
	pImpl->projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	pImpl->view = glm::mat4(1.0f);
}


Renderer::~Renderer()
{
	delete pImpl;
}

Model * Renderer::GetModel(std::string path)
{
	return pImpl->instanceManager.GetModel(path);
}

GLuint Renderer::GetShader(std::string name)
{
	return pImpl->shaderManager.GetShader(name);
}

void Renderer::GetProjection(glm::mat4 & perspective, glm::mat4 & view)
{
	perspective = pImpl->projection;
	view = pImpl->view;
}

ShaderManager Renderer::GetShaderManager()
{
	return pImpl->shaderManager;
}
