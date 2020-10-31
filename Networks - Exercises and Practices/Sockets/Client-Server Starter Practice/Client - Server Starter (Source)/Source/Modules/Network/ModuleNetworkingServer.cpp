#include "ModuleNetworkingServer.h"

// -- Delivery by Lucho Suaya and Sergi Parra --


// ------------------ ModuleNetworkingServer public methods ------------------
bool ModuleNetworkingServer::Start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// - Set address reuse
	// - Bind the socket to a local interface
	// - Enter in listen mode
	// - Add the listenSocket to the managed list of sockets using addSocket()

	// --- ---
	m_ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// If no error, set the class' socket
	if (m_ListeningSocket != INVALID_SOCKET)
	{
		// Set address reuse & check for errors -- TODO: Test if enable can be deleted or not
		int enable = 1;
		if (setsockopt(m_ListeningSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
			ReportError("[SERVER]: Couldn't set address reuse");
	
		// Build address
		sockaddr_in listenerAdd;
		listenerAdd.sin_family = AF_INET;
		listenerAdd.sin_port = htons(port);
		listenerAdd.sin_addr.S_un.S_addr = INADDR_ANY;
	
		// Bind socket to address and check for errors
		if (bind(m_ListeningSocket, (sockaddr*)&listenerAdd, sizeof(listenerAdd)) != SOCKET_ERROR)
		{
			// Enter in listen mode and check for errors
			if (listen(m_ListeningSocket, SOMAXCONN) != SOCKET_ERROR)
			{
				AddSocket(m_ListeningSocket);
				m_ServerState = ServerState::LISTENING;
			}
			else
				ReportErrorAndClose(m_ListeningSocket, "[SERVER]: Failed on listening mode enter in server socket", "SERVER SOCKET", "ModuleNetworkingServer::start()");
		}
		else
			ReportErrorAndClose(m_ListeningSocket, "[SERVER]: Failed binding server socket to address " + *listenerAdd.sin_zero + (":" + std::to_string(port)), "SERVER SOCKET", "ModuleNetworkingServer::start()");
	}
	else
		ReportErrorAndClose(m_ListeningSocket, "[SERVER]: Error opening socket m_ListeningSocket in ", "SERVER", "ModuleNetworkingServer::start()");

	
	// --- ---
	return true;
}

bool ModuleNetworkingServer::IsRunning() const
{
	return m_ServerState != ServerState::STOPPED;
}



// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingServer::Update()
{
	return true;
}

bool ModuleNetworkingServer::GUI()
{
	if (m_ServerState != ServerState::STOPPED)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("List of connected sockets:");

		for (ModuleNetworkingServer::ConnectedSocket &connectedSocket : m_ConnectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.client_name.c_str());
		}

		// Disconnect Button
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Disconnect"))
		{
			for (ConnectedSocket s : m_ConnectedSockets)
				m_DisconnectedSockets.push_back(s.socket);

			m_ServerDisconnection = true;
			m_ServerState = ServerState::STOPPED;
			App->modScreen->SwapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	m_ConnectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	ClientMessage clientMessage;
	packet >> clientMessage;

	if (clientMessage == ClientMessage::HELLO)
	{
		std::string clientName;
		packet >> clientName;

		for (ConnectedSocket& s : m_ConnectedSockets)
		{
			if (s.socket == socket)
			{
				s.client_name = clientName;
				LOG("Received client name from client '%s'", s.client_name.c_str()); // TODO: Delete this, is just for debugging

				OutputMemoryStream response;
				response << ServerMessage::WELCOME << "WELCOME CLIENT " << s.client_name;
				response << 0.5f << 0.2f << 0.1f;

				SendPacket(response, socket);
			}
		}
	}
}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	// Remove the connected socket from the list
	for (auto it = m_ConnectedSockets.begin(); it != m_ConnectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			IN_ADDR inAddr = connectedSocket.address.sin_addr;
			std::string add = std::to_string(inAddr.S_un.S_un_b.s_b1) + "." + std::to_string(inAddr.S_un.S_un_b.s_b2) + "." + std::to_string(inAddr.S_un.S_un_b.s_b3)
								+ "." + std::to_string(inAddr.S_un.S_un_b.s_b4) + ":" + std::to_string(ntohs(connectedSocket.address.sin_port));

			DEBUG_LOG("Disconnected client '%s' with address %s and ID: %d", connectedSocket.client_name.c_str(), add.c_str(), connectedSocket.socket);
			m_ConnectedSockets.erase(it);
			break;
		}
	}
}