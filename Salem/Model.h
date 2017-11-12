#pragma once
#include <string>
#include <glm\glm.hpp>
#include "Renderer.h"
using namespace std;

struct MeshData;
struct Material;

class Model
{
public:
	Model(string path);
	~Model();

	void Render(Renderer *r, glm::mat4 modelMatrix);
private:
	struct impl;

	impl *pImpl;
};

