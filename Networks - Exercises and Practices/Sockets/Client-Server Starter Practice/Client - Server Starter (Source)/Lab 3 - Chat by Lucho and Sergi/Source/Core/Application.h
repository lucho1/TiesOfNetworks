#pragma once

class Application
{
public:

	// Constructor and destructor
	Application();
	~Application();

	// Application methods
	bool WantsToExit() const { return wannaExit; }
	void ExitApp() { wannaExit = true; }

	// Application lifetime methods
	bool AppInit();
	bool AppUpdate();
	bool AppCleanUp();


private:

	// Private lifetime methods
	bool ModulesPreUpdate();
	bool ModulesUpdate();
	bool ModulesGUI();	
	bool ModulesPostUpdate();


public:

	// Modules
	ModulePlatform *modPlatform = nullptr;
	ModuleTaskManager *modTaskManager = nullptr;
	ModuleNetworkingClient *modNetClient = nullptr;
	ModuleNetworkingServer *modNetServer = nullptr;
	ModuleTextures *modTextures = nullptr;
	ModuleResources *modResources = nullptr;
	ModuleGameObject *modGameObject = nullptr;
	ModuleScreen *modScreen = nullptr;
	ModuleUI *modUI = nullptr;
	ModuleRender *modRender = nullptr;
	ModuleGamesManager* modGames = nullptr;


private:

	// All modules
	Module* modules[16] = {};
	int numModules = 0;

	// Exit flag
	bool wannaExit = false;
};

extern Application* App;