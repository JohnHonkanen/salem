#include "GBuffer.h"
#include "GL\glew.h"
#include <iostream>

struct GBuffer::impl {
	GLuint gBuffer;
	GLuint textures[3];
	GLuint rboDepth;
	int ScreenWdith, ScreenHeight;
	void ConfigureGBuffer();
};

GBuffer::GBuffer()
{
	pImpl = new impl();
}

GBuffer::GBuffer(unsigned int width, unsigned int height)
{
	pImpl = new impl();
	pImpl->ScreenHeight = height;
	pImpl->ScreenWdith = width;
}


GBuffer::~GBuffer()
{
	delete pImpl;
}

void GBuffer::Init()
{
	pImpl->ConfigureGBuffer();
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pImpl->gBuffer);
}

void GBuffer::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, pImpl->gBuffer);
}

void GBuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

void GBuffer::impl::ConfigureGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);

	glGenTextures(3,textures);
	glGenTextures(1, &rboDepth);

	for (unsigned int i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ScreenWdith, ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, rboDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, ScreenWdith, ScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rboDepth, 0);


	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, DrawBuffers);

	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
