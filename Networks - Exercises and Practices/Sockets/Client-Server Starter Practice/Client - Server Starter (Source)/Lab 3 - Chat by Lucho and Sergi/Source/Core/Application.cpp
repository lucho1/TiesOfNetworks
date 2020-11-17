#include "Core.h"

#define ADD_MODULE(ModuleClass, module) \
	module = new ModuleClass(); \
	modules[numModules++] = module;

Application::Application()
{
	// Create modules
	ADD_MODULE(ModulePlatform,			modPlatform);
	ADD_MODULE(ModuleRender,			modRender);
	ADD_MODULE(ModuleNetworkingClient,	modNetClient);
	ADD_MODULE(ModuleNetworkingServer,	modNetServer);
	ADD_MODULE(ModuleTaskManager,		modTaskManager);
	ADD_MODULE(ModuleTextures,			modTextures);
	ADD_MODULE(ModuleResources,			modResources);
	ADD_MODULE(ModuleGameObject,		modGameObject);
	ADD_MODULE(ModuleScreen,			modScreen);
	ADD_MODULE(ModuleUI,				modUI);
	ADD_MODULE(ModuleGamesManager,		modGames);
}


Application::~Application()
{
	// Destroy modules
	for (int i = 0; i < numModules; ++i)
		delete modules[i];
}


bool Application::AppInit()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->Init())
			return false;

	return true;
}

bool Application::AppUpdate()
{
	if (!ModulesPreUpdate())		return false;
	if (!ModulesUpdate())		return false;
	if (!ModulesGUI())			return false;
	if (!ModulesPostUpdate())	return false;

	modRender->Present();
	return true;
}

bool Application::AppCleanUp()
{
	for (int i = numModules; i > 0; --i)
		if (!modules[i - 1]->CleanUp())
			return false;

	return true;
}

bool Application::ModulesPreUpdate()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->PreUpdate())
			return false;

	return true;
}

bool Application::ModulesUpdate()
{
	unsigned int count = 0;
	static float accumulator = 0.0f;
	accumulator += Time.frameTime;

	while (accumulator >= Time.deltaTime)
	{
		accumulator -= Time.deltaTime;
		count++;

		for (int i = 0; i < numModules; ++i)
			if (!modules[i]->Update())
				return false;
	}

	// Interpolation of world values.
	// This is not an easy one... as we advance the simulation in
	// fixed time steps, the simulated results (positions, etc) are
	// a small amount of time ahead from the current time. To solve
	// this the ideal solution would be to interpolate all the values
	// between the previous state and the current state using this
	// deviation in time.

	return true;
}

bool Application::ModulesGUI()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->GUI())
			return false;

	return true;
}

bool Application::ModulesPostUpdate()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->PostUpdate())
			return false;

	return true;
}