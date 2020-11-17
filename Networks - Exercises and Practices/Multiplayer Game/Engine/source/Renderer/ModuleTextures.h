#ifndef _MODULE_TEXTURES_H_
#define _MODULE_TEXTURES_H_

struct ID3D11ShaderResourceView;

struct Texture
{
	ID3D11ShaderResourceView* shaderResource = nullptr;
	const char* filename = "";
	vec2 size = vec2{ -1.0f };
	bool used = false;
};

class ModuleTextures : public Module
{
public:

	// Module virtual functions
	virtual bool CleanUp() override;


	// ModuleTextures methods
	Texture* LoadTexture(const char *filename);
	Texture* LoadTexture(void *pixels, int width, int height);
	void FreeTexture(Texture *texture);


private:

	ID3D11ShaderResourceView *LoadD3DTextureFromFile(const char *filename, int *width, int *height);
	ID3D11ShaderResourceView *LoadD3DTextureFromPixels(void *pixels, int width, int height);
	Texture& GetTextureSlotForFilename(const char *filename);


private:

	Texture m_Textures[MAX_TEXTURES];
};


#endif //_MODULE_TEXTURES_H_