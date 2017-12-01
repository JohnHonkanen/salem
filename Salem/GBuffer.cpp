#include "GBuffer.h"
#include "GL\glew.h"
#include <iostream>

struct GBuffer::impl {
	GLuint gBuffer;
	GLuint gPosition, gNormal, gAlbedoSpec, gEmission;
	GLuint rboDepth;
	int screenWidth, screenHeight;
	void ConfigureGBuffer();
};

GBuffer::GBuffer()
{
	pImpl = new impl();
}

GBuffer::GBuffer(unsigned int width, unsigned int height)
{
	pImpl = new impl();
	pImpl->screenHeight = height;
	pImpl->screenWidth = width;
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

void GBuffer::GetTextures(unsigned int & position, unsigned int & normal, unsigned int & albedoSpec, unsigned int & emission)
{
	position = pImpl->gPosition;
	normal = pImpl->gNormal;
	albedoSpec = pImpl->gAlbedoSpec;
	emission = pImpl->gEmission;
}

void GBuffer::impl::ConfigureGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - color + Emission color buffer
	glGenTextures(1, &gEmission);
	glBindTexture(GL_TEXTURE_2D, gEmission);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmission, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, attachments);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
