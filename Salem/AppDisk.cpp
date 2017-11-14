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
	unique_ptr<GBuffer> gBuffer;

	unique_ptr<FrameBuffer> positionBuffer;
	unique_ptr<FrameBuffer> diffuseBuffer;
	unique_ptr<FrameBuffer> normalBuffer;
	
	

	GLuint quadVAO;

	int windowWidth = 1280;
	int windowHeight = 720;
	
	void RenderGeometryPass(const char* shader);
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
	pImpl->positionBuffer = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight);
	pImpl->diffuseBuffer = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight);
	pImpl->normalBuffer = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight);
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
	pImpl->positionBuffer->Init();
	pImpl->diffuseBuffer->Init();
	pImpl->normalBuffer->Init();

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
	pImpl->positionBuffer->BindForWriting();
	pImpl->RenderGeometryPass("gPosition");

	pImpl->diffuseBuffer->BindForWriting();
	pImpl->RenderGeometryPass("gDiffuse");

	pImpl->normalBuffer->BindForWriting();
	pImpl->RenderGeometryPass("gNormal");
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

void AppDisk::impl::RenderGeometryPass(const char *shader)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render(renderer.get(), shader);
	}
}

void AppDisk::impl::RenderLightPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Bind Default FrameBuffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(renderer->GetShader("lightPass"));

	unsigned int gPosition, gNormal, gAlbedoSpec;

	positionBuffer->BindForReading();
	gPosition = positionBuffer->GetTexture();

	normalBuffer->BindForReading();
	gNormal = normalBuffer->GetTexture();

	diffuseBuffer->BindForReading();
	gAlbedoSpec = diffuseBuffer->GetTexture();

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
	/*for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render(renderer.get());
	}*/
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
