#include "Core.h"
#include "ScreenMainMenu.h"

void ScreenMainMenu::Enable()
{
}

void ScreenMainMenu::GUI()
{
	ImGui::Begin("Main Menu");
	
	// --- Top Image ---
	Texture *banner = App->modResources->banner;
	ImVec2 bannerSize(400.0f, 400.0f * banner->height / banner->width);
	ImGui::Image(banner->shaderResource, bannerSize);

	ImGui::Spacing();

	// --- Server UI ---
	ImGui::Text("Server");

	static int localServerPort = 8888;
	ImGui::InputInt("Server Port Opened", &localServerPort);

	if (ImGui::Button("Start Server"))
	{
		App->modScreen->screenGame->SetAsServer(true);
		App->modScreen->screenGame->SetServerPort(localServerPort);

		App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenGame);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// --- Client UI ---
	ImGui::Text("Client");

	static char serverAddressStr[64] = "127.0.0.1";
	ImGui::InputText("Server Address", serverAddressStr, sizeof(serverAddressStr));

	static int remoteServerPort = 8888;
	ImGui::InputInt("Server Port to Connect", &remoteServerPort);

	static char playerNameStr[64] = "Username";
	ImGui::InputText("Player Name", playerNameStr, sizeof(playerNameStr));

	if (ImGui::Button("Connect to Server"))
	{
		App->modScreen->screenGame->SetAsServer(false);
		App->modScreen->screenGame->SetServerPort(remoteServerPort);
		App->modScreen->screenGame->SetServerAddress(serverAddressStr);
		App->modScreen->screenGame->SetClientName(playerNameStr);

		App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenGame);
	}

	ImGui::End();
}