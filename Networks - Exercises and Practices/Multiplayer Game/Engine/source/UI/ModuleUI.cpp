#include "Core/Core.h"
#include "ModuleUI.h"


extern HWND hwnd;                                // Window handle
extern ID3D11Device        *g_pd3dDevice;        // Direct3d11 device pointer
extern ID3D11DeviceContext *g_pd3dDeviceContext; // Direct3d11 device context pointer

// --- Virtual Methods ---
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

bool ModuleUI::DrawGUI()
{
	ImGui::Begin("Logging / Profiling");

	if (ImGui::BeginTabBar("Tab bar"))
	{
		if (ImGui::BeginTabItem("Console"))
		{
			// Messages Logging
			for (auto& msg_pair : m_ChatMessages)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(msg_pair.second.r, msg_pair.second.g, msg_pair.second.b, msg_pair.second.a));
				ImGui::TextWrapped("%s", msg_pair.first.c_str());
				ImGui::PopStyleColor();
			}

			ImGui::SetScrollHere(1.0f);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Profiling"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
			ImGui::Text("DEBUG CYCLE COUNTS");
			ImGui::Separator();

			for (int i = 0; i < DebugCycleCounter_Count; ++i)
			{
				if (DebugCycleCountersFront[i].hitCount > 0)
				{
					DebugCycleCounter it_cycle = DebugCycleCountersFront[i];
					ImGui::Text(" - %16s: %9I64u cy | %4d hits | %9I64u cy/hit", it_cycle.label, it_cycle.cycleCount, it_cycle.hitCount, it_cycle.cycleCount/it_cycle.hitCount);
				}
			}

			ImGui::PopStyleColor();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
	ImGui::End();
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


// --- Public Methods ---
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT ModuleUI::HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_InputsEnabled)
		return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

	return ERROR_SUCCESS;
}
