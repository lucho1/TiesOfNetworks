#include "Core/Core.h"


bool ModuleScreen::Init()
{
	m_ScreenCount = 0;
	m_Screens[m_ScreenCount++] = screenLoading  = new ScreenLoading;
	m_Screens[m_ScreenCount++] = screenBackground = new ScreenBackground;
	m_Screens[m_ScreenCount++] = screenMainMenu = new ScreenMainMenu;
	m_Screens[m_ScreenCount++] = screenGame = new ScreenGame;
	m_Screens[m_ScreenCount++] = screenOverlay  = new ScreenOverlay;

	screenLoading->enabled = true;
	screenBackground->enabled = true;
	return true;
}

bool ModuleScreen::Update()
{
	for (int i = 0; i < m_ScreenCount; ++i)
	{
		Screen *screen = m_Screens[i];
		if (!screen->enabled && screen->m_WasEnabled)
		{
			screen->Disable();
			screen->m_WasEnabled = screen->enabled;
			//App->modGameObject->deleteGameObjectsInScene(screen);
		}
	}

	for (int i = 0; i < m_ScreenCount; ++i)
	{
		Screen *screen = m_Screens[i];
		if (screen->enabled && !screen->m_WasEnabled)
		{
			screen->Enable();
			screen->m_WasEnabled = screen->enabled;
		}
	}

	for (int i = 0; i < m_ScreenCount; ++i)
	{
		Screen *screen = m_Screens[i];
		bool screenIsFullyEnabled = screen->enabled && screen->m_WasEnabled;
		if (screenIsFullyEnabled && screen->shouldUpdate)
			screen->Update();
	}

	return true;
}

bool ModuleScreen::DrawGUI()
{
	for (int i = 0; i < m_ScreenCount; ++i)
	{
		Screen *scene = m_Screens[i];
		if (scene->enabled)
			scene->DrawGUI();
	}

	return true;
}

bool ModuleScreen::CleanUp()
{
	for (int i = 0; i < m_ScreenCount; ++i)
	{
		Screen *scene = m_Screens[i];
		delete scene;
	}

	return true;
}

void ModuleScreen::SwapScreensWithTransition(Screen *oldScene, Screen *newScene)
{
	ASSERT(oldScene != nullptr && newScene != nullptr);
	ASSERT(screenOverlay->enabled == false);

	screenOverlay->oldScene = oldScene;
	screenOverlay->newScene = newScene;
	screenOverlay->enabled = true;
}