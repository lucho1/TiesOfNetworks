#pragma once

struct Texture;

class ModuleRender : public Module
{
public:

	// Virtual functions of Module
	bool init() override;
	bool postUpdate() override;
	bool cleanUp() override;

	// Public methods
	void resizeBuffers(unsigned int width, unsigned int height);
	void present();

	
private:

	// Private Methods
	void renderScene(int minOrder, int maxOrder);

	// D3D Methods
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();


private:

	Texture * whitePixel = nullptr;
	Texture * blackPixel = nullptr;
	uint8 shaderSource[Kilobytes(128)];

	GameObject* orderedGameObjects[MAX_GAME_OBJECTS] = {};
};