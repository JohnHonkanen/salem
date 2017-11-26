#pragma once

#include <vector>

using namespace std;

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(unsigned int width, unsigned int height, unsigned int attachmentCount);
	FrameBuffer(unsigned int width, unsigned int height, unsigned int attachmentCount, bool depthMap);
	~FrameBuffer();

	void Init();

	void BindForWriting();
	void BindForReading();

	void GetTexture(vector<unsigned int>& textures);
	GLuint GetHeight(unsigned int SHADOW_HEIGHT);
	GLuint GetWidth(unsigned int SHADOW_WIDTH);
	unsigned int GetTexture();
private:
	struct impl;
	impl * pImpl;
};

