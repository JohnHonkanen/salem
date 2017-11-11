#include "Renderer.h"
#include "InstanceManager.h"

struct Renderer::impl {
	InstanceManager instanceManager;

	
};

Renderer::Renderer()
{
	pImpl = new impl();
}


Renderer::~Renderer()
{
	delete pImpl;
}

Model * Renderer::GetModel(std::string path)
{
	return pImpl->instanceManager.GetModel(path);
}
