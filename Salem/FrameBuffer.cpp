#include "FrameBuffer.h"
#include <GL\glew.h>
#include <iostream>


struct FrameBuffer::impl {
	unsigned int fbo;
	unsigned int depthBuffer;
	unsigned int screenWidth, screenHeight, attachmentCount;
	void ConfigureFBO();
	bool depthMap = false;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024; // Resolution of the depth map

	vector<unsigned int> textureBuffer;
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

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, unsigned int count, bool depthMap)
{
	pImpl = new impl;
	pImpl->screenWidth = width;
	pImpl->screenHeight = height;
	pImpl->attachmentCount = count;
	pImpl->textureBuffer.resize(count);
	pImpl->depthMap = depthMap;
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
	textures = pImpl->textureBuffer;
}

const unsigned int FrameBuffer::GetHeight()
{
	return pImpl->SHADOW_HEIGHT;
}

const unsigned int FrameBuffer::GetWidth()
{
	return pImpl->SHADOW_WIDTH;
}

unsigned int FrameBuffer::GetTexture()
{
	return pImpl->textureBuffer[0];
}

void FrameBuffer::impl::ConfigureFBO()
{

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	// - position color buffer
	glGenTextures(attachmentCount, &textureBuffer[0]);

	vector<unsigned int> attachments;
	attachments.resize(attachmentCount);

	if (depthMap == true) {
		glBindTexture(GL_TEXTURE_2D, textureBuffer[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = {
			1.0f, 1.0f, 1.0f, 1.0f
		};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// attach depth texture as FBO's depth buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureBuffer[0], 0);

		// we only need the depth information when rendering the scene from the light's perspective, so there is no need for a color buffer to render any color data.
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		
	}
	else {
		for (int i = 0; i < attachmentCount; i++) {

			glBindTexture(GL_TEXTURE_2D, textureBuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureBuffer[i], 0);

			// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(attachmentCount, &attachments[0]);
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	}

	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
