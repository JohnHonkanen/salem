#include "FrameBuffer.h"
#include <GL\glew.h>
#include <iostream>


struct FrameBuffer::impl {
	unsigned int fbo;
	unsigned int depthBuffer;
	unsigned int screenWidth, screenHeight, attachmentCount;
	void ConfigureFBO(int attachmentCount);

	vector<unsigned int> colorBuffer;
};

FrameBuffer::FrameBuffer()
{
	pImpl = new impl;
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) : FrameBuffer(width, height, 1)
{

}


FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, unsigned int count)
{
	pImpl = new impl;
	pImpl->screenWidth = width;
	pImpl->screenHeight = height;
	pImpl->attachmentCount = count;
	pImpl->colorBuffer.resize(count);
}


FrameBuffer::~FrameBuffer()
{
	delete pImpl;
}

void FrameBuffer::Init(int count)
{
	pImpl->attachmentCount = count;

	if (pImpl->attachmentCount == 1) {

		pImpl->ConfigureFBO(count);
	}
	else {
		std::cout << "num attach higher than 1" << std::endl;
	}
}

void FrameBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pImpl->fbo);
}

void FrameBuffer::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, pImpl->fbo);
}

void FrameBuffer::GetTexture(vector<unsigned int>& textures)
{
	textures = pImpl->colorBuffer;
}

unsigned int FrameBuffer::GetTexture()
{
	return pImpl->colorBuffer[0];
}

void FrameBuffer::impl::ConfigureFBO(int count)
{

	attachmentCount = count;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	// - position color buffer
	glGenTextures(attachmentCount, &colorBuffer[0]);

	vector<unsigned int> attachments;
	attachments.resize(attachmentCount);
	for (int i = 0; i < attachmentCount; i++) {

		
		glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffer[i], 0);
		
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;

	}
	glDrawBuffers(attachmentCount, &attachments[0]);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
