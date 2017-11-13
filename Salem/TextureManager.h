#pragma once
#include <GL\glew.h>
#include <string>

using namespace std;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	GLuint GetTexture(string texture);
private:
	struct impl;
	impl* pImpl;
};

