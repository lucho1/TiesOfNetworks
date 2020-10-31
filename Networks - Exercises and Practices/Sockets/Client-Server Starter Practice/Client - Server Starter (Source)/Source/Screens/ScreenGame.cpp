#include "Core.h"

void ScreenGame::Enable()
{
	if (isServer)
	{
		if (!App->modNetServer->Start(m_ServerPort))
			WARN_LOG("Could not start ModuleNetworkingServer");
	}
	else
	{
		if (!App->modNetClient->Start(m_ServerAddress, m_ServerPort, m_ClientName))
			WARN_LOG("Could not start ModuleNetworkingClient");
	}
}

void ScreenGame::Update()
{
	if (isServer)
	{
		if (!App->modNetServer->IsRunning())
			App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenMainMenu);
	}
	else
	{
		if (!App->modNetClient->IsRunning())
			App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenMainMenu);
	}
}