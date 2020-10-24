#include "ModuleNetworkingClient.h"

// ------------------ ModuleNetworkingClient public methods ------------------
bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char * clientName)
{
	m_ClientName = clientName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	// - Create the remote address object
	// - Connect to the remote address
	// - Add the created socket to the managed list of sockets using addSocket()

	// If everything was ok... change the state
	m_ClientState = ClientState::Start;

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return m_ClientState != ClientState::Stopped;
}



// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingClient::update()
{
	if (m_ClientState == ClientState::Start)
	{
		// TODO(jesus): Send the player name to the server
	}

	return true;
}

bool ModuleNetworkingClient::gui()
{
	if (m_ClientState != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("%s connected to the server...", m_ClientName.c_str());

		ImGui::End();
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, byte * data)
{
	m_ClientState = ClientState::Stopped;
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	m_ClientState = ClientState::Stopped;
}