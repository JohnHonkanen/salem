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

	int shadowWidth = 1024;
	int shadowHeight = 1024;

	float near_plane = 1.0f;
	float far_plane = 25.0f;
	mat4 lightSpaceMatrix;

	float angle = 0;

	bool showSpotLight = true;

	void RenderGeometryPass();
	void RenderLightPass();

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
	pImpl->shadowBuffer = make_unique<FrameBuffer>(pImpl->shadowWidth, pImpl->shadowHeight, 1, true);
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
	/*Light for Shadow*/
	//Move PointLight
	vec3 center = vec3(10.0f, 0.0f, 25.0f);
	float dist = 8.0f;

	float x = cos(pImpl->angle) * dist;
	float z = sin(pImpl->angle) *dist;

	pImpl->pointLights[0].position = vec3(x, 10, z) + center;

	pImpl->angle += 1.0f * dt;
	/*Light for Shadow*/
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
	/*Do Shadow Pass*/
	pImpl->RenderShadowPass();
	/* Do Deferred Rendering Passes */
	/* Do Geometry Pass*/
	pImpl->RenderGeometryPass();
	/* Do Light Pass */
	pImpl->RenderLightPass();
	/*-------------------------------*/

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

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_F]) {
		pImpl->showSpotLight = true;
	}
	if (keys[SDL_SCANCODE_G]) {
		pImpl->showSpotLight = false;
	}

	if (keys[SDL_SCANCODE_R]) {
		for (int i = 1; i < pImpl->pointLights.size()-1; i++) {
			pImpl->pointLights[i].ambient = vec3((rand() % 100) / 10.0f, (rand() % 100) / 10.0f, (rand() % 100) / 10.0f);
			pImpl->pointLights[i].diffuse = pImpl->pointLights[i].ambient;
		}
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
	glViewport(0, 0, windowWidth, windowHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render(renderer.get());
	}
}

void AppDisk::impl::RenderLightPass()
{
	lightBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(renderer->GetShader("lightPass"));

	unsigned int gPosition, gNormal, gAlbedoSpec, gEmission;

	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("lightPass");

	gBuffer->BindForReading();
	gBuffer->GetTextures(gPosition, gNormal, gAlbedoSpec, gEmission);

	shadowBuffer->BindForReading();
	unsigned int tex;
	tex = shadowBuffer->GetTexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gEmission);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tex);

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

	float cutOff = 12.5f, outerCutoff = 17.5f;
	if (!showSpotLight) {
		cutOff = 0.0f, outerCutoff = 0.0f;
	}
	// Spotlight Attenuation + inner/outer cutoff
	shaderManager->SetUniformLocation1f(program, "spotLight.constant", 1.0f);
	shaderManager->SetUniformLocation1f(program, "spotLight.linear", 0.9f);
	shaderManager->SetUniformLocation1f(program, "spotLight.quadratic", 0.32f);
	shaderManager->SetUniformLocation1f(program, "spotLight.cutOff", glm::cos(glm::radians(cutOff)));
	shaderManager->SetUniformLocation1f(program, "spotLight.outerCutOff", glm::cos(glm::radians(outerCutoff)));

	// Shadow/DepthMap 
	shaderManager->SetUniformLocation1f(program, "near_plane", near_plane);
	shaderManager->SetUniformLocation1f(program, "far_plane", far_plane);
	shaderManager->SetUniformLocation1i(program, "depthMap", 4);
	shaderManager->SetUniformMatrix4fv(program, "lightSpaceMatrix", lightSpaceMatrix);

	RenderQuad();
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
	glViewport(0, 0, shadowBuffer->GetWidth(), shadowBuffer->GetHeight());

	shadowBuffer->BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	/* 1) Configure matrices and shader mat4 uniforms*/
	
	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("depthMap");

	glUseProgram(program);

	mat4 lightProjection, lightView;

	vec3 lightPos = pointLights[0].position; 
	vec3 objPosi = vec3(10.0f, 0.0f, 25.0f);
	vec3 vectorDif = normalize(objPosi - lightPos);

	//lightProjection = glm::perspective(glm::radians(45.0f), (float)(shadowWidth / shadowHeight), near_plane, far_plane);
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, lightPos + vectorDif, glm::vec3(0.0f, 1.0f, 0.0f));
	
	lightSpaceMatrix = lightProjection * lightView;

	shaderManager->SetUniformMatrix4fv(program, "lightSpaceMatrix", lightSpaceMatrix);

	
	objects[0]->Render(renderer.get(), "depthMap");


	glViewport(0, 0, windowWidth, windowHeight); // Reset viewport to (Screen width and height)
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
	//shadowBuffer->BindForReading();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //Write to default

	ShaderManager* shaderManager = renderer->GetShaderManager();
	unsigned int program = renderer->GetShader("default");

	glUseProgram(program);

	unsigned int texture;

	// Read HDRBuffer data 
	texture = HDRBuffer->GetTexture();
	//texture = shadowBuffer->GetTexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shaderManager->SetUniformLocation1i(program, "texture0", 0);

	RenderQuad();
}
