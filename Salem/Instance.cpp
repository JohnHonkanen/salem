#include "Instance.h"
#include "InstanceManager.h"
#include "ShaderManager.h"
#include <glm\glm.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace glm;

struct Instance::impl {
	int amount = 0;
	vector<mat4> modelMatrices;
	string path;
	Model * instance;

	string shader = "geometry";

	void ConfigureInstance();
};


Instance::Instance() : pImpl{ new impl() }
{
}

Instance::Instance(string path)
{
	pImpl = new impl();
	pImpl->path = path;
}

Instance::~Instance()
{
	delete pImpl;
}

void Instance::Render(Renderer * r)
{
	glm::mat4 projection, view;

	r->GetProjection(projection, view);

	vector<MeshData> data = pImpl->instance->GetData();
	vector<GLuint> VAOs = pImpl->instance->GetVAO();

	ShaderManager * shaderManager = r->GetShaderManager();
	GLuint program = r->GetShader(pImpl->shader);
	glUseProgram(program);

	for (int i = 0; i < VAOs.size(); i++) {
		
		shaderManager->SetUniformMatrix4fv(program, "projection", projection);
		shaderManager->SetUniformMatrix4fv(program, "view", view);

		glBindVertexArray(VAOs[i]);
		glDrawElementsInstanced(GL_TRIANGLES, data[i].indexCount, GL_UNSIGNED_INT, 0, pImpl->amount);
		glBindVertexArray(0);
	}
}

void Instance::AddInstance(glm::mat4 transform)
{
	pImpl->modelMatrices.push_back(transform);
	pImpl->amount++;
}

void Instance::Init(InstanceManager * manager)
{
	pImpl->instance = manager->GetModel(pImpl->path);
	pImpl->instance->GetMaterial()[0].shader = pImpl->shader;
	pImpl->ConfigureInstance();
}
void Instance::impl::ConfigureInstance()
{

	vector<MeshData> data = instance->GetData();
	vector<GLuint> VAOs = instance->GetVAO();

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (int i = 0; i < data.size(); i++) {
		GLuint VAO = VAOs[i];
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}
}
