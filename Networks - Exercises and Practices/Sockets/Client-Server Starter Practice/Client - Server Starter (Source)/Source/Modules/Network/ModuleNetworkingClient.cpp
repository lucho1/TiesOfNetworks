#include "ModuleNetworkingClient.h"

// -- Delivery by Lucho Suaya and Sergi Parra --

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
		m_ServerAddressStr = serverAddressStr + std::string(":") + std::to_string(serverPort);
		m_ServerAddress.sin_family = AF_INET;
		m_ServerAddress.sin_port = htons(serverPort);
		inet_pton(AF_INET, serverAddressStr, &m_ServerAddress.sin_addr);
	
		if (connect(m_Socket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) != SOCKET_ERROR)
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
		OutputMemoryStream packet;
		packet << ClientMessage::HELLO;
		packet << m_ClientName;

		if (SendPacket(packet, m_Socket))
			m_ClientState = ClientState::Logging;
		else
		{
			ERROR_LOG(std::string("[CLIENT]: Error sending data to server '" + m_ServerAddressStr + "' from " + m_ClientName + " client on ModuleNetworkingClient::update()").c_str());
			disconnect();
			m_ClientState = ClientState::Stopped;
		}
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

		ImGui::Text("'%s' connected to the server '%s'...", m_ClientName.c_str(), m_ServerAddressStr.c_str());

		// Disconnect Button
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine();
		ImGui::Separator();
		if (ImGui::Button("Disconnect"))
		{
			m_DisconnectedSockets.push_back(m_Socket);
			m_ClientState = ClientState::Stopped;
			App->modScreen->swapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	ServerMessage serverMessage;
	packet >> serverMessage;

	std::string welcome_msg;
	packet >> welcome_msg;	
	std::string cName;
	packet >> cName;
	welcome_msg += cName;

	float r, g, b;
	packet >> r; packet >> g; packet >> b;

	if (serverMessage == ServerMessage::WELCOME)
		LOG("RECEIVED WELCOME MESSAGE FROM SERVER '%s'\n\tIt says: %s\n\tYour new color will be: %.2f, %.2f, %.2f", m_ServerAddressStr.c_str(), welcome_msg.c_str(), r, g, b);

	m_ClientState = ClientState::Stopped;
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	DEBUG_LOG("Socket at '%s' Disconnected from Server '%s'", m_ClientName.c_str(), m_ServerAddressStr.c_str());
	m_DisconnectedSockets.push_back(m_Socket);
	
	if (shutdown(socket, 2) == SOCKET_ERROR)
		ReportErrorAndClose(socket, { "[CLIENT]: Error shuting down the socket of client '" + m_ClientName + "'"}, m_ClientName + " CLIENT", "ModuleNetworkingClient::onSocketDisconnected()");
	else if (closesocket(socket) == SOCKET_ERROR)
		reportError(std::string("[CLIENT]: Error Closing socket from '" + m_ClientName + "' Client on function ModuleNetworkingClient::onSocketDisconnected()").c_str());

	m_ClientState = ClientState::Stopped;
}