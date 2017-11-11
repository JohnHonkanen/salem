#pragma once
#include <string>

class Model;

class Renderer
{
public:
	Renderer();
	~Renderer();

	Model* GetModel(std::string path);

private:

	struct impl;
	impl* pImpl;
};

