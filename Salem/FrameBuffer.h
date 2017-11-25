#pragma once
class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(unsigned int width, unsigned int height, unsigned int attachmentCount);
	~FrameBuffer();

	void Init(int numAttachment);

	void BindForWriting();
	void BindForReading();

	unsigned int GetTexture();
private:
	struct impl;
	impl * pImpl;
};

