#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class Application
{
public:

	// --- Constructor and destructor ---
	Application();
	~Application();


	// --- Application Methods ---
	bool AppInit();
	bool AppUpdate();
	bool AppCleanUp();
	void AppExit() { m_ExitFlag = true; }


private:

	// --- Private App Methods --> For Modules ---
	bool DoStart();
	bool DoPreUpdate();
	bool DoUpdate();
	bool DoDrawGUI();
	bool DoPostUpdate();
	bool DoStop();


public:

	// Modules
	ModulePlatform*			modPlatform = nullptr;
	ModuleTaskManager*		modTaskManager = nullptr;
	//ModuleNetworking*		modNet = nullptr;
	ModuleNetworkingServer*	modNetServer = nullptr;
	ModuleNetworkingClient*	modNetClient = nullptr;
	ModuleLinkingContext*	modLinkingContext = nullptr;
	ModuleTextures*			modTextures = nullptr;
	ModuleResources*		modResources = nullptr;
	ModuleGameObject*		modGameObject = nullptr;
	ModuleCollision*		modCollision = nullptr;
	ModuleBehaviour*		modBehaviour = nullptr;
	ModuleSound*			modSound = nullptr;
	ModuleScreen*			modScreen = nullptr;
	ModuleUI*				modUI = nullptr;
	ModuleRender*			modRender = nullptr;


private:

	// All modules
	static const int MAX_MODULES = 16;
	Module* m_Modules[MAX_MODULES] = {};
	int m_NumModules = 0;

	// Exit flag
	bool m_ExitFlag = false;
};

extern Application* App;

#endif //_APPLICATION_H_