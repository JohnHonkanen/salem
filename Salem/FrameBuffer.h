#pragma once
class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(unsigned int width, unsigned int height);
	~FrameBuffer();

	void Init();

	void BindForWriting();
	void BindForReading();

	unsigned int GetTexture();
private:
	struct impl;
	impl * pImpl;
};

