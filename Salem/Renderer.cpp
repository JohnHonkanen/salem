#include "Renderer.h"
#include "InstanceManager.h"
#include "ShaderManager.h"
#include <glm\gtc\matrix_transform.hpp>
#include <memory>
#include "Camera.h"
#include "TextureManager.h"

struct Renderer::impl {
	std::unique_ptr<InstanceManager> instanceManager;
	std::unique_ptr<ShaderManager> shaderManager;
	std::unique_ptr<TextureManager> textureManager;
	glm::mat4 projection;
	

};

Renderer::Renderer()
{
	pImpl = new impl();
	pImpl->shaderManager = std::make_unique<ShaderManager>();
	pImpl->instanceManager = std::make_unique<InstanceManager>();
	pImpl->projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	pImpl->textureManager = std::make_unique<TextureManager>();
}


Renderer::~Renderer()
{
	delete pImpl;
}

Model * Renderer::GetModel(std::string path)
{
	return pImpl->instanceManager->GetModel(path);
}

GLuint Renderer::GetShader(std::string name)
{
	return pImpl->shaderManager->GetShader(name);
}

void Renderer::GetProjection(glm::mat4 & perspective, glm::mat4 & view)
{
	perspective = pImpl->projection;
	view = camera.GetView();
}

ShaderManager * Renderer::GetShaderManager()
{
	return pImpl->shaderManager.get();
}

InstanceManager * Renderer::GetInstanceManager()
{
	return pImpl->instanceManager.get();
}

TextureManager * Renderer::GetTextureManager()
{
	return pImpl->textureManager.get();
}
