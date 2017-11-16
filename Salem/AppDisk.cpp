#include "AppDisk.h"
#include "Object.h"
#include "Renderer.h"
#include "GBuffer.h"
#include "FrameBuffer.h"
#include "ShaderManager.h"
#include <vector>
#include <memory>

using namespace std;
typedef unique_ptr<Object> ObjectUP;

struct AppDisk::impl {
	unique_ptr<Renderer> renderer;
	vector<ObjectUP> objects;
	vector<ObjectUP> fObjects;
	unique_ptr<GBuffer> gBuffer;

	GLuint quadVAO;

	int windowWidth = 1280;
	int windowHeight = 720;
	
	void RenderGeometryPass();
	void RenderLightPass();
	void PointLightPass();

	void RenderForward();

	void RenderQuad();
};

AppDisk::AppDisk()
{
	pImpl = new impl();
	pImpl->renderer = make_unique<Renderer>();
	pImpl->gBuffer = make_unique<GBuffer>(pImpl->windowWidth, pImpl->windowHeight);
}


AppDisk::~AppDisk()
{
	delete pImpl;
}

void AppDisk::Start()
{
	ShaderManager *shaderManager = pImpl->renderer->GetShaderManager();
	GLuint lightPass = pImpl->renderer->GetShader("lightPass");
	glUseProgram(lightPass);
	shaderManager->SetUniformLocation1i(lightPass, "gPosition", 0);
	shaderManager->SetUniformLocation1i(lightPass, "gNormal", 1);
	shaderManager->SetUniformLocation1i(lightPass, "gAlbedoSpec", 2);

	pImpl->gBuffer->Init();
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

Object * AppDisk::AddObject(std::string path, bool deferred, const char* shader)
{
	ObjectUP object = make_unique<Object>(pImpl->renderer->GetModel(path));
	object->SetShader(shader);
	if (deferred) {
		pImpl->objects.push_back(std::move(object));
		return pImpl->objects.back().get();
	}
	else {
		pImpl->fObjects.push_back(std::move(object));
		return pImpl->fObjects.back().get();
	}

	
}

Object * AppDisk::AddObject(Object * object, bool deferred, const char* shader)
{
	ObjectUP o = ObjectUP(object);
	o->Init(pImpl->renderer->GetInstanceManager());
	o->SetShader(shader);
	if (deferred) {
		pImpl->objects.push_back(std::move(o));
		return pImpl->objects.back().get();
	}
	else {
		pImpl->fObjects.push_back(std::move(o));
		return pImpl->fObjects.back().get();
	}
	
}

void AppDisk::impl::RenderGeometryPass()
{
	gBuffer->BindForWriting();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render(renderer.get());
	}
}

void AppDisk::impl::RenderLightPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Bind Default FrameBuffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(renderer->GetShader("lightPass"));

	gBuffer->BindForReading();

	unsigned int gPosition, gNormal, gAlbedoSpec;
	gBuffer->GetTextures(gPosition, gNormal, gAlbedoSpec);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	RenderQuad();

	gBuffer->BindForReading();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //Write to default

	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void AppDisk::impl::PointLightPass()
{
}

void AppDisk::impl::RenderForward()
{
	for (int i = 0; i < fObjects.size(); i++) {
		fObjects[i]->Render(renderer.get());
	}
}

void AppDisk::impl::RenderQuad()
{
	unsigned int quadVBO;
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
