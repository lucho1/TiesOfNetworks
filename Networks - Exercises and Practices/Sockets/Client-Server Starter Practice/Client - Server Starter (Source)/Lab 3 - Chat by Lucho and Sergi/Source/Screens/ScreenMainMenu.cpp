#include "Core.h"
#include "ScreenMainMenu.h"
#include <regex>

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

	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank;

	static int localServerPort = 8888;
	ImGui::InputInt("Port To Open", &localServerPort, flags);

	static char serverNameStr[64] = "ServerName";
	ImGui::InputText("Server Name", serverNameStr, sizeof(serverNameStr), flags);

	if (ImGui::Button("Start Server"))
	{
		App->modScreen->screenGame->SetAsServer(true);
		App->modScreen->screenGame->SetServerPort(localServerPort);
		App->modScreen->screenGame->SetServerName(serverNameStr);

		App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenGame);
		App->modUI->ClearConsoleMessages();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// --- Client UI ---
	ImGui::Text("Client");

	static char serverAddressStr[64] = "127.0.0.1";
	ImGui::InputText("Server Address", serverAddressStr, sizeof(serverAddressStr), flags | ImGuiInputTextFlags_CharsDecimal);

	static int remoteServerPort = 8888;
	ImGui::InputInt("Port to Connect", &remoteServerPort, flags);
	
	static char playerNameStr[64] = "Username";
	ImGui::InputText("Player Name", playerNameStr, sizeof(playerNameStr), flags);

	if (ImGui::Button("Connect to Server"))
	{
		std::regex reg("(.*)[<>'\\/\"`´](.*)");

		if (std::regex_match(playerNameStr, reg))
			APPCONSOLE_WARN_LOG("Invalid characters in your name!");
		else {
			App->modScreen->screenGame->SetAsServer(false);
			App->modScreen->screenGame->SetServerPort(remoteServerPort);
			App->modScreen->screenGame->SetServerAddress(serverAddressStr);
			App->modScreen->screenGame->SetClientName(playerNameStr);

			App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenGame);
			App->modUI->ClearConsoleMessages();
		}
	}

	ImGui::End();
}