#define _USE_MATH_DEFINES

#include "FireFly.h"
#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <cmath>
#include <ctime>
#include <SDL.h>

#include "ShaderManager.h"
#include "TextureManager.h"
#include "Renderer.h"


using namespace std;
using namespace glm;

struct FireFly::impl {
	string texture = "Assets/Textures/particle.png";
	const char *shader;
	unsigned int vao;
	unsigned int vbo[2];
	int particles = 5;
	vector<float> vertices;
	vector<float> velocities;
	vector<float> timepassed;
	vector<float> check;

	int velocity = 10;
	float clamps = 1.0f;


	float timePassed = 0;

	void Setup();
	void SetupVertices();
};

FireFly::FireFly()
{
	srand(time(NULL));
	pImpl = new impl;

	pImpl->Setup();
}


FireFly::~FireFly()
{
	glDeleteVertexArrays(0, &pImpl->vao);
	glDeleteBuffers(2, pImpl->vbo);
	delete pImpl;
}

void FireFly::Render(Renderer * r)
{
	glm::mat4 projection, view;

	r->GetProjection(projection, view);

	ShaderManager *sm = r->GetShaderManager();
	TextureManager* tm = r->GetTextureManager();

	GLuint program = sm->GetShader(pImpl->shader);

	glUseProgram(program);

	sm->SetUniformMatrix4fv(program, "projection", projection);
	sm->SetUniformMatrix4fv(program, "view", view);
	sm->SetUniformMatrix4fv(program, "model", modelMatrix);

	unsigned int pTex = tm->GetTexture(pImpl->texture);
	// Bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pTex);
	sm->SetUniformLocation1i(program, "texture0", 0);

	glPointSize(10);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);


	glBindVertexArray(pImpl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pImpl->particles*3 * sizeof(float), &pImpl->vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, pImpl->particles);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void FireFly::Update(float dt)
{
	for (int i = 0; i < pImpl->particles*3; i++) {
		pImpl->timepassed[i] += dt;

		if (pImpl->timepassed[i] > pImpl->check[i])
		{
			float v;
			v = (rand() % pImpl->velocity - pImpl->velocity/2) / 10.0f;
			pImpl->velocities[i] = v;
			pImpl->timepassed[i] = 0.0f;
		}

		pImpl->vertices[i] += pImpl->velocities[i] * dt;

		pImpl->vertices[i] = clamp(pImpl->vertices[i], -pImpl->clamps, pImpl->clamps);
	}
}

void FireFly::SetShader(const char * shader)
{
	pImpl->shader = shader;
}

void FireFly::impl::Setup()
{
	SetupVertices();
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);

	glBindVertexArray(vao);
	/* Vertex position*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()/3 * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 , 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void FireFly::impl::SetupVertices()
{
	double PI = 3.141592653589793238462643383279502884197;
	double seg = PI / 2.0f;

	for (int i = 0; i < particles; i++) {
		
		float x, y, z;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;

		float vx, vy, vz;
		vx = (rand() % 100 - 50) / 10.0f;
		vy = (rand() % 100 - 50) / 10.0f;
		vz = (rand() % 100 - 50) / 10.0f;

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);

		velocities.push_back(vx);
		velocities.push_back(vy);
		velocities.push_back(vz);

		timepassed.push_back(0);
		timepassed.push_back(0);
		timepassed.push_back(0);

		float c = (rand() % 50 + 50) / 100.0f;
		check.push_back(c);
		check.push_back(c);
		check.push_back(c);

	}
}
