#include "AppDisk.h"
#include "Object.h"
#include "Renderer.h"
#include <vector>
#include <memory>
using namespace std;
typedef unique_ptr<Object> ObjectUP;

struct AppDisk::impl {
	unique_ptr<Renderer> renderer;
	vector<ObjectUP> objects;
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
	for (int i = 0; i < pImpl->objects.size(); i++) {
		pImpl->objects[i]->Rotate(glm::vec3(0.01f, 0.01f,0.0f));
	}
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
	ObjectUP object = make_unique<Object>(pImpl->renderer->GetModel(path));
	object->Translate(glm::vec3(0.0f, 0.0f, -3.00f));
	pImpl->objects.push_back(std::move(object));
}
