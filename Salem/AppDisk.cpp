#include "AppDisk.h"
#include "Model.h"
#include "Renderer.h"
#include <vector>
#include <memory>
using namespace std;


struct AppDisk::impl {
	Renderer renderer;
	vector<string> objects;
};

AppDisk::AppDisk()
{
	pImpl = new impl();
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
		pImpl->renderer.GetModel(pImpl->objects[i])->Render(pImpl->renderer);
	}
}

void AppDisk::Input()
{
}

void AppDisk::addObject(std::string path)
{
	pImpl->objects.push_back(path);
}
