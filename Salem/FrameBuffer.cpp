#include "FrameBuffer.h"
#include <GL\glew.h>
#include <iostream>


struct FrameBuffer::impl {
	unsigned int fbo;
	unsigned int depthBuffer;
	unsigned int screenWidth, screenHeight, attachmentCount;

	bool depthOnly;
	void ConfigureFBO();

	vector<unsigned int> colorBuffer;
};

FrameBuffer::FrameBuffer()
{
	pImpl = new impl;
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) : FrameBuffer(width, height, 1)
{

}


FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, unsigned int count) : FrameBuffer(width, height, count, false)
{
	
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, unsigned int attachmentCount, bool depthOnly)
{
	pImpl = new impl;
	pImpl->screenWidth = width;
	pImpl->screenHeight = height;
	pImpl->attachmentCount = attachmentCount;
	pImpl->colorBuffer.resize(attachmentCount);
	pImpl->depthOnly = depthOnly;
}


FrameBuffer::~FrameBuffer()
{
	delete pImpl;
}

void FrameBuffer::Init()
{
	pImpl->ConfigureFBO();
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

void FrameBuffer::impl::ConfigureFBO()
{

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	// - position color buffer
	glGenTextures(attachmentCount, &colorBuffer[0]);

	vector<unsigned int> attachments;
	attachments.resize(attachmentCount);

	if (!depthOnly) {
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
	}
	else {
		glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, colorBuffer[0], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	



	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
