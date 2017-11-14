#include "TextureManager.h"
#include <map>
#include <iostream>
#include <IL\il.h>

struct TextureManager::impl {

	void SaveTexture(const string texture);
	map<string, GLuint> textures;
};

TextureManager::TextureManager()
{
	pImpl =new impl();
}


TextureManager::~TextureManager()
{
	delete pImpl;
}

GLuint TextureManager::GetTexture(string texture)
{
	auto it = pImpl->textures.find(texture);

	if (it != pImpl->textures.end()) {
		return pImpl->textures[texture];
	}

	// Create Texture

	pImpl->SaveTexture(texture);
	return pImpl->textures[texture];
}

void TextureManager::impl::SaveTexture(const string texture)
{

	// Generate DevIL image loader

	ILuint width;
	ILuint height;
	ILuint image;

	if (ilLoadImage((const ILstring)texture.c_str())) {

	}
	else {
		cout << ilGetError() << endl;
		cout << "Failed to load image: " << texture.c_str() << endl;
	}

	// Convert image to usable image type
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	// Get image width and height
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	// OpenGL texture loading
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set texture wrapper parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters (Mipmap)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Texture image converted to linear space (GL_SRGB_ALPHA) 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	// Generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	ilDeleteImages(1, &image); // Deletes image
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

	textures.insert(pair<string, GLuint>(texture, textureID));
}
