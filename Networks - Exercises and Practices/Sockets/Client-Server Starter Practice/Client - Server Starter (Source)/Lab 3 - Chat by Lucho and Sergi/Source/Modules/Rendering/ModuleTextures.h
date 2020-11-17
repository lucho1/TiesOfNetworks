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
	virtual bool CleanUp() override;

	// Public Functions
	Texture* LoadTexture(const char *filename);
	Texture* LoadTexture(void *pixels, int width, int height);
	void FreeTexture(Texture *texture);


private:

	// Private Functions
	ID3D11ShaderResourceView* LoadD3DTextureFromFile(const char *filename, int *width, int *height);
	ID3D11ShaderResourceView* LoadD3DTextureFromPixels(void *pixels, int width, int height);
	Texture& GetTextureSlotForFilename(const char *filename);


private:

	Texture m_Textures[MAX_TEXTURES];
};