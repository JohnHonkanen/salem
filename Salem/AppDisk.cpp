#include "AppDisk.h"
#include "Model.h"
#include "Renderer.h"
#include <vector>
#include <memory>
using namespace std;


struct AppDisk::impl {
	unique_ptr<Renderer> renderer;
	vector<Model *> objects;
};

AppDisk::AppDisk()
{
	pImpl = new impl();
	pImpl->renderer = make_unique<Renderer>();
}


AppDisk::~AppDisk()
{
	delete pImpl;
}

void AppDisk::Update()
{
}

void AppDisk::Render()
{
	for (int i = 0; i < pImpl->objects.size(); i++) {
		pImpl->objects[i]->Render(pImpl->renderer.get());
	}
}

void AppDisk::Input()
{
}

void AppDisk::addObject(std::string path)
{
	pImpl->objects.push_back(pImpl->renderer->GetModel(path));
}
