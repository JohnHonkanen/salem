#pragma once
#include <string>
#include "Renderer.h"
using namespace std;

struct MeshData;
struct Material;

class Model
{
public:
	Model(string path);
	~Model();

	void Render(Renderer r);
private:
	struct impl;

	impl *pImpl;
};

