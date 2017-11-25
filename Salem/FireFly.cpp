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
#include "Renderer.h"


using namespace std;
using namespace glm;

struct FireFly::impl {

	const char *shader;
	unsigned int vao;
	unsigned int vbo[2];
	vector<float> vertices;
	vector<float> velocities;

	int timePassed;

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

	GLuint program = sm->GetShader(pImpl->shader);

	glUseProgram(program);

	sm->SetUniformMatrix4fv(program, "projection", projection);
	sm->SetUniformMatrix4fv(program, "view", view);
	sm->SetUniformMatrix4fv(program, "model", modelMatrix);

	glPointSize(10);
	glBindVertexArray(pImpl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pImpl->vertices.size() * sizeof(float), &pImpl->vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, pImpl->vertices.size());
	glBindVertexArray(0);
}

void FireFly::Update(float dt)
{
	double PI = 3.141592653589793238462643383279502884197;
	double seg = PI / 2.0f;

	pImpl->vertices.clear();

	for (double phi = 0; phi < PI * 2; phi += seg) {
		for (double theta = 0; theta < PI; theta += seg) {
			float r = (rand() % 20) / 10.0f;
			float x, y, z;
			x = r * cos(phi) * sin(theta);
			y = r * sin(phi) * sin(theta);
			z = r * cos(theta);

			pImpl->vertices.push_back(x);
			pImpl->vertices.push_back(y);
			pImpl->vertices.push_back(z);
		}
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 , 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void FireFly::impl::SetupVertices()
{
	double PI = 3.141592653589793238462643383279502884197;
	double seg = PI / 2.0f;

	for (int i = 0; i < 5; i++) {
		
		float x, y, z;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;

		float vx, vy, vz;
		vx = rand() % 10 - 5;
		vy = rand() % 10 - 5;
		vz = rand() % 10 - 5;

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
	}
}
