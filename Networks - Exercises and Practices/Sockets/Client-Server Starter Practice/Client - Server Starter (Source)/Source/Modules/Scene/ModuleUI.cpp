#include "Core/Core.h"


extern HWND hwnd;                                // Window handle
extern ID3D11Device        *g_pd3dDevice;        // Direct3d11 device pointer
extern ID3D11DeviceContext *g_pd3dDeviceContext; // Direct3d11 device context pointer

Texture *banner = nullptr;

bool ModuleUI::Init()
{
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	banner = App->modTextures->LoadTexture("Assets/Textures/banner.jpg");
	return true;
}

bool ModuleUI::PreUpdate()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	return true;
}

bool ModuleUI::PostUpdate()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return true;
}

bool ModuleUI::CleanUp()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	return true;
}

bool ModuleUI::GUI()
{
	ImGui::Begin("Logging window");

	const char* console_name = "Clear Console";
	if (App->modNetClient->IsRunning())
		console_name = "Clear Chat";

	if (ImGui::Button(console_name))
		App->modUI->ClearConsoleMessages();

	for (auto& msg_pair : m_ChatMessages)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(msg_pair.second.r, msg_pair.second.g, msg_pair.second.b, msg_pair.second.a));
		ImGui::TextWrapped("%s", msg_pair.first.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::End();

	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ModuleUI::HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
}