#include "AppDisk.h"
#include "Object.h"
#include "Renderer.h"
#include "GBuffer.h"
#include "FrameBuffer.h"
#include "ShaderManager.h"

#include <vector>
#include <memory>
#include <string>

using namespace std;
typedef unique_ptr<Object> ObjectUP;

struct AppDisk::impl {
	unique_ptr<Renderer> renderer;
	vector<ObjectUP> objects;
	vector<ObjectUP> fObjects;
	unique_ptr<GBuffer> gBuffer;
	unique_ptr<FrameBuffer> HDRBuffer;
	unique_ptr<FrameBuffer> lightBuffer;
	unique_ptr<FrameBuffer> pingPongBuffer[2];
	unique_ptr<FrameBuffer> shadowBuffer;

	vector<PointLight> pointLights;

	GLuint quadVAO;

	bool horizontal = true;

	int windowWidth = 1280;
	int windowHeight = 720;

	void RenderGeometryPass();
	void RenderLightPass();
	void PointLightPass();

	void RenderForward();

	void RenderQuad();

	void RenderShadowPass();
	void RenderBloomPass();
	void RenderHDRPass();
	void RendererFinalImage();
};

AppDisk::AppDisk()
{
	pImpl = new impl();
	pImpl->renderer = make_unique<Renderer>();
	pImpl->gBuffer = make_unique<GBuffer>(pImpl->windowWidth, pImpl->windowHeight);
	pImpl->HDRBuffer = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight);
	pImpl->lightBuffer = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight, 2);
	pImpl->pingPongBuffer[0] = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight);
	pImpl->pingPongBuffer[1] = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight);
	pImpl->shadowBuffer = make_unique<FrameBuffer>(pImpl->windowWidth, pImpl->windowHeight, 1, true);
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
	shaderManager->SetUniformLocation1i(lightPass, "gEmission", 3);

	pImpl->gBuffer->Init();
	pImpl->HDRBuffer->Init();
	pImpl->lightBuffer->Init();
	pImpl->pingPongBuffer[0]->Init();
	pImpl->pingPongBuffer[1]->Init();
	pImpl->shadowBuffer->Init();
}

void AppDisk::Update(float dt)
{
	pImpl->renderer->camera.Update(dt);
	for (int i = 0; i < pImpl->objects.size(); i++) {
		pImpl->objects[i]->Update(dt);
	}

	for (int i = 0; i < pImpl->fObjects.size(); i++) {
		pImpl->fObjects[i]->Update(dt);
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

	/*Do Shadow Pass*/
	pImpl->RenderShadowPass();

	/*Do Bloom Pass*/
	pImpl->RenderBloomPass();
	/*-------------------------------*/

	/*Do HDR Pass to tone map*/
	pImpl->RenderHDRPass();
	/*-------------------------------*/
	pImpl->RendererFinalImage();

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

void AppDisk::AddPointLights(PointLight light)
{
	pImpl->pointLights.push_back(light);
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
	//glBindFramebuffer(GL_FRAMEBUFFER, 0); //Bind Default FrameBuffer
	lightBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(renderer->GetShader("lightPass"));

	unsigned int gPosition, gNormal, gAlbedoSpec, gEmission;

	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("lightPass");

	gBuffer->BindForReading();

	gBuffer->GetTextures(gPosition, gNormal, gAlbedoSpec, gEmission);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gEmission);

	// View position of camera

	glm::vec3 cameraPosition = renderer->camera.GetModelMatrix()[3];
	glm::vec3 cameraDirection = -renderer->camera.Front();

	// View position to be passed into vertex shader
	glm::vec3 viewPosition = renderer->camera.GetModelMatrix()[3];
	shaderManager->SetUniformLocation3f(program, "viewPosi",
		cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Light position to be passed into vertex shader
	glm::vec3 lightPosition = renderer->camera.GetModelMatrix()[3];
	shaderManager->SetUniformLocation3f(program, "lightPosi",
		cameraPosition.x, cameraPosition.y, cameraPosition.z);

	/**********POINTLIGHT PROPERTIES**********/

	//// Camera
	//shaderManager->SetUniformLocation3f(program, "pointLight.position",
	//	15.0f, 6.0f, -5.0f);

	//// Pointlight Uniforms 
	//shaderManager->SetUniformLocation3f(program, "pointLight.ambient", 0.5f, 0.5f, 0.5f);
	//shaderManager->SetUniformLocation3f(program, "pointLight.diffuse", 0.5f, 0.5f, 0.5f);
	//shaderManager->SetUniformLocation3f(program, "pointLight.specular", 0.15f, 0.15f, 0.15f);

	//// Pointlight Attenuation
	//shaderManager->SetUniformLocation1f(program, "pointLight.constant", 1.0f);
	//shaderManager->SetUniformLocation1f(program, "pointLight.linear", 0.1f);
	//shaderManager->SetUniformLocation1f(program, "pointLight.quadratic", 3.0f);

	shaderManager->SetUniformLocation1i(program, "totalLights", pointLights.size());

	for (int i = 0; i < pointLights.size(); i++) {
		shaderManager->SetUniformLocation3f(program, "pointLight["+ std::to_string(i) +"].position",
			pointLights[i].position.x, pointLights[i].position.y, pointLights[i].position.z);

		// Pointlight Uniforms 
		shaderManager->SetUniformLocation3f(program, "pointLight[" + std::to_string(i) + "].ambient", pointLights[i].ambient.x, pointLights[i].ambient.y, pointLights[i].ambient.z);
		shaderManager->SetUniformLocation3f(program, "pointLight[" + std::to_string(i) + "].diffuse", pointLights[i].diffuse.x, pointLights[i].diffuse.y, pointLights[i].diffuse.z);
		shaderManager->SetUniformLocation3f(program, "pointLight[" + std::to_string(i) + "].specular", pointLights[i].specular.x, pointLights[i].specular.y, pointLights[i].specular.z);

		// Pointlight Attenuation
		shaderManager->SetUniformLocation1f(program, "pointLight[" + std::to_string(i) + "].constant", pointLights[i].constant);
		shaderManager->SetUniformLocation1f(program, "pointLight[" + std::to_string(i) + "].linear", pointLights[i].linear);
		shaderManager->SetUniformLocation1f(program, "pointLight[" + std::to_string(i) + "].quadratic", pointLights[i].quadratic);
	}

	/**********SPOTLIGHT PROPERTIES**********/

	// Camera
	shaderManager->SetUniformLocation3f(program, "spotLight.direction",
		cameraDirection.x, cameraDirection.y, cameraDirection.z);

	cameraPosition = renderer->camera.GetModelMatrix()[3];
	shaderManager->SetUniformLocation3f(program, "spotLight.position",
		cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Spotlight Uniforms 
	shaderManager->SetUniformLocation3f(program, "spotLight.ambient", 0.005f, 0.005f, 0.005f);
	shaderManager->SetUniformLocation3f(program, "spotLight.diffuse", 0.5f, 0.5f, 0.5f);
	//shaderManager->SetUniformLocation3f(program, "spotLight.diffuse", 0.8f, 0.8f, 0.8f);
	shaderManager->SetUniformLocation3f(program, "spotLight.specular", 0.5f, 0.5f, 0.5f);

	// Spotlight Attenuation + inner/outer cutoff: Range currently set at: 3250
	shaderManager->SetUniformLocation1f(program, "spotLight.constant", 1.0f);
	shaderManager->SetUniformLocation1f(program, "spotLight.linear", 0.9f); // 0.0014f
	shaderManager->SetUniformLocation1f(program, "spotLight.quadratic", 0.32f); // 0.000007f
	shaderManager->SetUniformLocation1f(program, "spotLight.cutOff", glm::cos(glm::radians(12.5f))); //glm::cos(glm::radians(5.5f * 1.2f))); // 12.5
	shaderManager->SetUniformLocation1f(program, "spotLight.outerCutOff", glm::cos(glm::radians(17.5f))); //glm::cos(glm::radians(12.5f * 1.2f))); // 17.5

																										  // Material Uniforms + Properties

																										  //shaderManager->SetUniformLocation3f(program, "diffuse", 1.0f, 0.5f, 0.31f);
																										  //shaderManager->SetUniformLocation3f(program, "specular", 0.5f, 0.5f, 0.5f);

	RenderQuad();
}

void AppDisk::impl::PointLightPass()
{
}

void AppDisk::impl::RenderForward()
{
	//Copy Depth Buffer
	gBuffer->BindForReading();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(
		0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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


void AppDisk::impl::RenderShadowPass()
{
	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("shadow");

	glUseProgram(program);

	vector<unsigned int >texture;

	// Read Lightbuffer data 
	lightBuffer->BindForReading();
	lightBuffer->GetTexture(texture);

	glViewport(0, 0, shadowBuffer->GetWidth(), shadowBuffer->GetHeight());
	shadowBuffer->BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);

	/* 1) Configure matrices and shader mat4 uniforms*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]); // bind texture of depth map.

	/* 2) Render scene as normal with shadow mapping (using depth map)*/
	glViewport(0, 0, 1080, 720); // Reset viewport to (Screen width and height)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, texture[0]); // bind texture of depth map.
	RenderQuad(); // Render scene
}

void AppDisk::impl::RenderBloomPass()
{

	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("BloomPass");

	glUseProgram(program);

	vector<unsigned int >texture;

	// Read Lightbuffer data 
	lightBuffer->BindForReading();
	lightBuffer->GetTexture(texture);

	bool firstIteration = true;
	int count = 10;

	for (unsigned int i = 0; i < count; i++) {

		pingPongBuffer[horizontal]->BindForWriting();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderManager->SetUniformLocation1i(program, "image", 0);
		shaderManager->SetUniformLocation1i(program, "Horizontal", horizontal);

		GLuint tex = texture[1];

		if (!firstIteration) {
			tex = pingPongBuffer[!horizontal]->GetTexture();
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex); // bind texture of other framebuffer (or scene if first iteration)

		RenderQuad();
		horizontal = !horizontal;

		if (firstIteration) {
			firstIteration = false;
		}
	}
}

void AppDisk::impl::RenderHDRPass()
{
	// Apply tone mapping.

	HDRBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(renderer->GetShader("HDRPass"));

	vector<unsigned int >texture1, texture2;

	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("HDRPass");

	// Read Lightbuffer data 
	lightBuffer->BindForReading();
	lightBuffer->GetTexture(texture1);

	// Read pingPong buffer data
	pingPongBuffer[!horizontal]->GetTexture(texture2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1[0]);

	shaderManager->SetUniformLocation1i(program, "HDR", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2[0]);

	shaderManager->SetUniformLocation1i(program, "bloomBlur", 1);

	RenderQuad();
}

void AppDisk::impl::RendererFinalImage()
{

	HDRBuffer->BindForReading();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //Write to default

	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("default");

	glUseProgram(program);

	unsigned int texture;

	// Read HDRBuffer data 
	texture = HDRBuffer->GetTexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shaderManager->SetUniformLocation1i(program, "texture0", 0);

	RenderQuad();
}
