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
	
		connect(m_Socket, (sockaddr*)&remote_address, sizeof(remote_address));
		addSocket(m_Socket);
	}
	else
	{
		reportError("[CLIENT]: Error connecting m_Socket to remote_address ModuleNetworkingClient::start()");
		if (closesocket(m_Socket) == SOCKET_ERROR)
			reportError("[CLIENT]: Error closing m_Socket on ModuleNetworkingClient::start()");
	}	
	
	m_ClientState = ClientState::Start; // If everything was ok... change the state
	
	//m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//m_ClientState = ClientState::Start;
	//bool closeSocket = false;
	//
	//if (m_Socket != INVALID_SOCKET)
	//{
	//	m_ServerAddress.sin_family = AF_INET;
	//	m_ServerAddress.sin_port = htons(serverPort);
	//	inet_pton(AF_INET, serverAddressStr, &m_ServerAddress.sin_addr);
	//
	//	if (connect(m_Socket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) != SOCKET_ERROR)
	//	{
	//		m_ClientState = ClientState::Start; // If everything was ok... change the state
	//		addSocket(m_Socket);
	//	}
	//	else
	//	{
	//		closeSocket = true;
	//		reportError("[CLIENT]: Error connecting m_Socket to remote_address ModuleNetworkingClient::start()");
	//	}
	//}
	//else
	//{
	//	closeSocket = true;
	//	reportError("[CLIENT]: Error connecting m_Socket to remote_address ModuleNetworkingClient::start()");
	//}
	//
	//if(closeSocket)
	//	if (closesocket(m_Socket) == SOCKET_ERROR)
	//		reportError("[CLIENT]: Error closing m_Socket on ModuleNetworkingClient::start()");



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
			LOG("Sent name %s", m_ClientName.c_str());
			m_ClientState = ClientState::Logging;
		}
		else
		{
			char err_msg[150];
			sprintf_s(err_msg, "[CLIENT]: Error sending data to server from %s client on ModuleNetworkingClient::update()", m_ClientName.c_str());
			reportError(err_msg);
		}

		//m_ClientState = ClientState::Start;
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