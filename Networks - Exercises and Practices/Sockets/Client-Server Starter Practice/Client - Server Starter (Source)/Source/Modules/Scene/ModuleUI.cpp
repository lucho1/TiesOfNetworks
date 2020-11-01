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

	if (ImGui::Button("Clear Console"))
		logLines.clear();

	for (uint32 entryIndex = 0; entryIndex < GetLogEntryCount(); ++entryIndex)
	{
		LogEntry entry = GetLogEntry(entryIndex);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(entry.text_color->r, entry.text_color->g, entry.text_color->b, entry.text_color->a));
		ImGui::TextWrapped("%s", entry.message);
		ImGui::PopStyleColor();
	}

	ImGui::End();
	return true;
}

inline void ModuleUI::PrintMessageInConsole(const char* msg, const Color& col) const
{
	PushLogEntry(LogEntry(msg, col));
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ModuleUI::HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
}