#include "Core/Core.h"


GameObject* spaceTopLeft = nullptr;
GameObject* spaceTopRight = nullptr;
GameObject* spaceBottomLeft = nullptr;
GameObject* spaceBottomRight = nullptr;

void ScreenGame::Enable()
{
	if (isServer)
	{
		App->modNetServer->SetListenPort(serverPort);
		App->modNetServer->Enable(true);
	}
	else
	{
		App->modNetClient->SetServerAddress(serverAddress, serverPort);
		App->modNetClient->SetPlayerInfo(playerName, spaceshipType);
		App->modNetClient->Enable(true);
	}

	spaceTopLeft = Instantiate();
	spaceTopLeft->sprite = App->modRender->AddSprite(spaceTopLeft);
	spaceTopLeft->sprite->texture = App->modResources->space;
	spaceTopLeft->sprite->order = -1;

	spaceTopRight = Instantiate();
	spaceTopRight->sprite = App->modRender->AddSprite(spaceTopRight);
	spaceTopRight->sprite->texture = App->modResources->space;
	spaceTopRight->sprite->order = -1;

	spaceBottomLeft = Instantiate();
	spaceBottomLeft->sprite = App->modRender->AddSprite(spaceBottomLeft);
	spaceBottomLeft->sprite->texture = App->modResources->space;
	spaceBottomLeft->sprite->order = -1;

	spaceBottomRight = Instantiate();
	spaceBottomRight->sprite = App->modRender->AddSprite(spaceBottomRight);
	spaceBottomRight->sprite->texture = App->modResources->space;
	spaceBottomRight->sprite->order = -1;
}

void ScreenGame::Update()
{
	if (!(App->modNetServer->IsConnected() || App->modNetClient->IsConnected()))
		App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenMainMenu);
	else
	{
		if (!isServer)
		{
			vec2 camPos = App->modRender->cameraPosition;
			vec2 bgSize = spaceTopLeft->sprite->texture->size;

			spaceTopLeft->position = bgSize * floor(camPos / bgSize);
			spaceTopRight->position = bgSize * (floor(camPos / bgSize) + vec2{ 1.0f, 0.0f });
			spaceBottomLeft->position = bgSize * (floor(camPos / bgSize) + vec2{ 0.0f, 1.0f });
			spaceBottomRight->position = bgSize * (floor(camPos / bgSize) + vec2{ 1.0f, 1.0f });;
		}
	}
}

void ScreenGame::DrawGUI()
{
}

void ScreenGame::Disable()
{
	Destroy(spaceTopLeft);
	Destroy(spaceTopRight);
	Destroy(spaceBottomLeft);
	Destroy(spaceBottomRight);
}
