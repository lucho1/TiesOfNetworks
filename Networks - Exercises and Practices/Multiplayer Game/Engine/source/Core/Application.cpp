#include "Core.h"
#include "Application.h"

#define ADD_MODULE(ModuleClass, module) \
	module = new ModuleClass(); \
	m_Modules[m_NumModules++] = module;

#define ADD_MODULE_DISABLED(ModuleClass, module) \
	module = new ModuleClass(); \
	module->Enable(false); \
	m_Modules[m_NumModules++] = module;


// --- Constructor and destructor ---
Application::Application()
{
	ADD_MODULE          (ModulePlatform,         modPlatform);
	ADD_MODULE          (ModuleRender,           modRender);
	ADD_MODULE          (ModuleSound,            modSound);
	ADD_MODULE_DISABLED (ModuleNetworkingServer, modNetServer);
	ADD_MODULE_DISABLED (ModuleNetworkingClient, modNetClient);
	ADD_MODULE          (ModuleLinkingContext,   modLinkingContext);
	ADD_MODULE          (ModuleTaskManager,      modTaskManager);
	ADD_MODULE          (ModuleTextures,         modTextures);
	ADD_MODULE          (ModuleResources,        modResources);
	ADD_MODULE          (ModuleGameObject,       modGameObject);
	ADD_MODULE          (ModuleCollision,        modCollision);
	ADD_MODULE          (ModuleBehaviour,        modBehaviour);
	ADD_MODULE          (ModuleScreen,           modScreen);
	ADD_MODULE          (ModuleUI,               modUI);
}


// --- Application Methods ---
Application::~Application()
{
	for (int i = 0; i < m_NumModules; ++i)
	{
		Module* module = m_Modules[i];
		delete module;
	}
}


// --- Application Lifetime Methods ---
bool Application::AppInit()
{
	for (int i = 0; i < m_NumModules; ++i)
	{
		Module* module = m_Modules[i];
		if (!module->Init())
			return false;
	}

	if (!DoStart())
		return false;

	return true;
}

bool Application::AppUpdate()
{
	BEGIN_TIMED_BLOCK(Frame);

	if (!DoStart())
		return false;

	if (!DoPreUpdate())
		return false;	

	if (!DoUpdate())
		return false;

	if (!DoDrawGUI())
		return false;

	if (!DoPostUpdate())
		return false;

	if (!DoStop())
		return false;

	modRender->Present();

	END_TIMED_BLOCK(Frame);
	END_PROFILING_FRAME();
	return (m_ExitFlag == false);
}

bool Application::AppCleanUp()
{
	for (int i = m_NumModules - 1; i >= 0; --i)
		m_Modules[i]->Enable(false);

	if (!DoStop())
		return false;

	for (int i = m_NumModules - 1; i >= 0; --i)
	{
		Module* module = m_Modules[i];
		if (!module->CleanUp())
			return false;
	}

	return true;
}


// --- Private App Methods --> For Modules ---
bool Application::DoStart()
{
	for (int i = 0; i < m_NumModules; ++i)
	{
		Module* module = m_Modules[i];
		if (module->NeedsStart())
		{
			module->UpdateEnabledState();
			if (!module->Start())
				return false;
		}
	}

	return true;
}

bool Application::DoPreUpdate()
{
	BEGIN_TIMED_BLOCK(PreUpdate);

	for (int i = 0; i < m_NumModules; ++i)
	{
		Module* module = m_Modules[i];
		if (module->IsEnabled())
		{
			if (!module->PreUpdate())
				return false;
		}
	}

	END_TIMED_BLOCK(PreUpdate);
	return true;
}

bool Application::DoUpdate()
{
	BEGIN_TIMED_BLOCK(Update);

	static float accumulator = 0.0f;
	accumulator += Time.frameTime;
	unsigned int count = 0;

	//while (accumulator >= Time.deltaTime)
	{
		accumulator -= Time.deltaTime;
		count++;

		for (int i = 0; i < m_NumModules; ++i)
		{
			Module* module = m_Modules[i];
			if (module->IsEnabled())
			{
				if (!module->Update())
					return false;
			}
		}
	}

	// TO DO: Interpolation of world values.
	// This is not an easy one... as we advance the simulation in
	// fixed time steps, the simulated results (positions, etc) are
	// a small amount of time ahead from the current time. To solve
	// this the ideal solution would be to interpolate all the values
	// between the previous state and the current state using this
	// deviation in time.

	END_TIMED_BLOCK(Update);
	return true;
}

bool Application::DoDrawGUI()
{
	BEGIN_TIMED_BLOCK(GUIDraw);

	if (modUI->IsEnabled())
	{
		for (int i = 0; i < m_NumModules; ++i)
		{
			Module* module = m_Modules[i];
			if (module->IsEnabled())
			{
				if (!module->DrawGUI())
					return false;
			}
		}
	}

	END_TIMED_BLOCK(GUIDraw);
	return true;
}

bool Application::DoPostUpdate()
{
	BEGIN_TIMED_BLOCK(PostUpdate);

	for (int i = 0; i < m_NumModules; ++i)
	{
		Module* module = m_Modules[i];

		if (module->IsEnabled())
		{
			if (!module->PostUpdate())
				return false;
		}
	}

	END_TIMED_BLOCK(PostUpdate);
	return true;
}

bool Application::DoStop()
{
	for (int i = m_NumModules - 1; i >= 0; --i)
	{
		Module* module = m_Modules[i];
		if (module->NeedsStop())
		{
			module->UpdateEnabledState();
			if (!module->Stop())
				return false;
		}
	}

	return true;
}