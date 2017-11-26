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
	unsigned int GetTexture();
private:
	struct impl;
	impl * pImpl;
};

