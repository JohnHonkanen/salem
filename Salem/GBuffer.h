#pragma once
class GBuffer
{
public:

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();
	GBuffer(unsigned int width, unsigned int height);
	~GBuffer();

	void Init();
	void BindForWriting();
	void BindForReading();
	void SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);

	void GetTextures(unsigned int &position, unsigned int &normal, unsigned int &albedoSpec);
private:
	struct impl;

	impl* pImpl;
};

