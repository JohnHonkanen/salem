#include "AppDisk.h"
#include "Model.h"
#include "Renderer.h"
#include <vector>
#include <memory>
using namespace std;

struct AppDisk::impl {
	Renderer renderer;
	vector<unique_ptr<Model>> models;
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
	for (int i = 0; i < pImpl->models.size(); i++) {
		pImpl->models[i]->Render(pImpl->renderer);
	}
}

void AppDisk::Input()
{
}

void AppDisk::addObject(std::string path)
{
	pImpl->models.push_back(make_unique<Model>(path));
}
