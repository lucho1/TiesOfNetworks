#pragma once

struct Texture;

class ModuleRender : public Module
{
public:

	// Virtual functions of Module
	virtual bool Init() override;
	virtual bool PostUpdate() override;
	virtual bool CleanUp() override;

	// Public methods
	void ResizeBuffers(unsigned int width, unsigned int height);
	void Present();

	
private:

	// Private Methods
	void RenderScene(int minOrder, int maxOrder);

	// D3D Methods
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();


private:

	Texture* m_WhitePixel = nullptr;
	Texture* m_BlackPixel = nullptr;
	uint8 m_ShaderSource[Kilobytes(128)];

	GameObject* m_OrderedGameObjects[MAX_GAME_OBJECTS] = {};
};