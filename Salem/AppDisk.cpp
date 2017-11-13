#include "AppDisk.h"
#include "Object.h"
#include "Renderer.h"
#include "GBuffer.h"
#include <vector>
#include <memory>

using namespace std;
typedef unique_ptr<Object> ObjectUP;

struct AppDisk::impl {
	unique_ptr<Renderer> renderer;
	vector<ObjectUP> objects;
	unique_ptr<GBuffer> gBuffer;
	
	void RenderGeometryPass();
	void RenderLightPass();

	void RenderForward();
};

AppDisk::AppDisk()
{
	pImpl = new impl();
	pImpl->renderer = make_unique<Renderer>();
	pImpl->gBuffer = make_unique<GBuffer>(1280, 720);
}


AppDisk::~AppDisk()
{
	delete pImpl;
}

void AppDisk::Start()
{
	pImpl->gBuffer->Init();
	for (int i = 0; i < pImpl->objects.size(); i++) {
		pImpl->objects[i]->Init(pImpl->renderer->GetInstanceManager());
	}
}

void AppDisk::Update(float dt)
{
	pImpl->renderer->camera.Update(dt);
	for (int i = 0; i < pImpl->objects.size(); i++) {
		pImpl->objects[i]->Update();
	}
}

void AppDisk::Render()
{
	/* Do Deferred Rendering Passes */
	/* Do Geometry Pass*/
	pImpl->RenderGeometryPass();
	/* Do Light Pass */
	pImpl->RenderLightPass();
	/*-------------------------------*/

	/* Do Forward Rendering Passes  */
	pImpl->RenderForward();
	/*-------------------------------*/
}

void AppDisk::Input(SDL_Event* sdlEvent)
{
	pImpl->renderer->camera.Input(sdlEvent);
	for (int i = 0; i < pImpl->objects.size(); i++) {
		pImpl->objects[i]->Input();
	}
}

void AppDisk::AddObject(std::string path)
{
	ObjectUP object = make_unique<Object>(pImpl->renderer->GetModel(path));
	object->Translate(glm::vec3(0.0f, 0.0f, -3.00f));
	pImpl->objects.push_back(std::move(object));
}

void AppDisk::AddObject(Object * object)
{
	ObjectUP o = ObjectUP(object);
	pImpl->objects.push_back(std::move(o));
}

void AppDisk::impl::RenderGeometryPass()
{
}

void AppDisk::impl::RenderLightPass()
{
}

void AppDisk::impl::RenderForward()
{
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render(renderer.get());
	}
}
