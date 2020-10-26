#include "Networks.h"
#include "ScreenMainMenu.h"

void ScreenMainMenu::enable()
{
	//LOG("Example INFO log entry...");
	//TEXT_LOG("Example TEXT log entry...");
	//DEBUG_LOG("Example DEBUG log entry...");
	//WARN_LOG("Example WARNING log entry...");
	//ERROR_LOG("Example ERROR log entry...");
}

void ScreenMainMenu::gui()
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
		App->modScreen->screenGame->isServer = true;
		App->modScreen->screenGame->serverPort = localServerPort;
		App->modScreen->swapScreensWithTransition(this, App->modScreen->screenGame);
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

	static char playerNameStr[64] = "JesusGrandeeee";
	ImGui::InputText("Player Name", playerNameStr, sizeof(playerNameStr));

	if (ImGui::Button("Connect to Server"))
	{
		App->modScreen->screenGame->isServer = false;
		App->modScreen->screenGame->serverPort = remoteServerPort;
		App->modScreen->screenGame->serverAddress = serverAddressStr;
		App->modScreen->screenGame->playerName = playerNameStr;
		App->modScreen->swapScreensWithTransition(this, App->modScreen->screenGame);
	}

	ImGui::End();
}