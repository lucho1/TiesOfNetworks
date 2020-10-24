#include "Networks.h"

#define ADD_MODULE(ModuleClass, module) \
	module = new ModuleClass(); \
	modules[numModules++] = module;

Application::Application()
{
	// Create modules
	ADD_MODULE(ModulePlatform,         modPlatform);
	ADD_MODULE(ModuleRender,           modRender);
	ADD_MODULE(ModuleNetworkingClient, modNetClient);
	ADD_MODULE(ModuleNetworkingServer, modNetServer);
	ADD_MODULE(ModuleTaskManager,      modTaskManager);
	ADD_MODULE(ModuleTextures,         modTextures);
	ADD_MODULE(ModuleResources,        modResources);
	ADD_MODULE(ModuleGameObject,       modGameObject);
	ADD_MODULE(ModuleScreen,           modScreen);
	ADD_MODULE(ModuleUI,               modUI);
}


Application::~Application()
{
	// Destroy modules
	for (int i = 0; i < numModules; ++i)
		delete modules[i];
}


bool Application::init()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->init())
			return false;

	return true;
}

bool Application::update()
{
	if (!doPreUpdate())		return false;	
	if (!doUpdate())		return false;
	if (!doGui())			return false;
	if (!doPostUpdate())	return false;

	modRender->present();
	return true;
}

bool Application::cleanUp()
{
	for (int i = numModules; i > 0; --i)
		if (!modules[i - 1]->cleanUp())
			return false;

	return true;
}

bool Application::doPreUpdate()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->preUpdate())
			return false;

	return true;
}

bool Application::doUpdate()
{
	unsigned int count = 0;
	static float accumulator = 0.0f;
	accumulator += Time.frameTime;

	while (accumulator >= Time.deltaTime)
	{
		accumulator -= Time.deltaTime;
		count++;

		for (int i = 0; i < numModules; ++i)
			if (!modules[i]->update())
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

bool Application::doGui()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->gui())
			return false;

	return true;
}

bool Application::doPostUpdate()
{
	for (int i = 0; i < numModules; ++i)
		if (!modules[i]->postUpdate())
			return false;

	return true;
}