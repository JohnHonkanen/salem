#pragma once
#include <string>

using namespace std;

struct MeshData;
struct Material;

class Model
{
public:
	Model(string path);
	~Model();

	void Render();
private:
	struct impl;

	impl *pImpl;
};

