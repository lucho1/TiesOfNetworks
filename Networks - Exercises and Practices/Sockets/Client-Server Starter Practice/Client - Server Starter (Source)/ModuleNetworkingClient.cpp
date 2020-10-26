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
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if (m_Socket != INVALID_SOCKET)
	{
		sockaddr_in remote_address;
		remote_address.sin_family = AF_INET;
		remote_address.sin_port = htons(serverPort);
		inet_pton(AF_INET, serverAddressStr, &remote_address.sin_addr);
	
		if (connect(m_Socket, (sockaddr*)&remote_address, sizeof(remote_address)) != SOCKET_ERROR)
		{
			addSocket(m_Socket);
			m_ClientState = ClientState::Start; // If everything was ok... change the state
		}
		else
		{
			std::string msg = { "[CLIENT]: Failed connecting socket from " + m_ClientName + " to remote address " + serverAddressStr + ":" + std::to_string(serverPort) };
			ReportErrorAndClose(m_Socket, msg, "CLIENT" + m_ClientName, "ModuleNetworkingClient::start()");
		}
	}
	else
		ReportErrorAndClose(m_Socket, "[CLIENT]: Error opening socket m_Socket in ", m_ClientName + " Client", "ModuleNetworkingClient::start()");

	// --- ---
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
		if (send(m_Socket, m_ClientName.c_str(), m_ClientName.size() + 1, 0) != SOCKET_ERROR)
		{
			LOG("Sent client name from client '%s'", m_ClientName.c_str());
			m_ClientState = ClientState::Logging;
		}
		else
			reportError(std::string("[CLIENT]: Error sending data to server from " + m_ClientName + " client on ModuleNetworkingClient::update()").c_str());
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
	m_SocketsVec.erase(std::find(m_SocketsVec.begin(), m_SocketsVec.end(), socket));
	
	if (shutdown(socket, 2) == SOCKET_ERROR)
		ReportErrorAndClose(socket, { "[CLIENT]: Error shuting down the socket of client " + m_ClientName}, m_ClientName + "CLIENT", "ModuleNetworkingClient::onSocketDisconnected()");
	else if (closesocket(socket) == SOCKET_ERROR)
		reportError(std::string("[CLIENT]: Error Closing socket from '" + m_ClientName + "' Client on function ModuleNetworkingClient::onSocketDisconnected()").c_str());

	m_ClientState = ClientState::Stopped;
}