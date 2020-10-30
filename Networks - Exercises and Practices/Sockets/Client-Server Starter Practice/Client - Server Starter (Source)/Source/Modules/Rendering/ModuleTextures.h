#pragma once

struct ID3D11ShaderResourceView;

struct Texture
{
	ID3D11ShaderResourceView *shaderResource = nullptr;
	const char *filename = "";
	int width = -1;
	int height = -1;
	bool used = false;
};

class ModuleTextures : public Module
{
public:

	// Virtual functions of Module
	bool init() override;
	bool cleanUp() override;

	// Public Functions
	Texture* loadTexture(const char *filename);
	Texture *loadTexture(void *pixels, int width, int height);
	void freeTexture(Texture *texture);


private:

	// Private Functions
	ID3D11ShaderResourceView *loadD3DTextureFromFile(const char *filename, int *width, int *height);
	ID3D11ShaderResourceView *loadD3DTextureFromPixels(void *pixels, int width, int height);
	Texture & getTextureSlotForFilename(const char *filename);


private:

	Texture _textures[MAX_TEXTURES];
};