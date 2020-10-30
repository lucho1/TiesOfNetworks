#include "ModuleNetworkingServer.h"

// -- Delivery by Lucho Suaya and Sergi Parra --


// ------------------ ModuleNetworkingServer public methods ------------------
bool ModuleNetworkingServer::start(int port)
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
			reportError("[SERVER]: Couldn't set address reuse");
	
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
				addSocket(m_ListeningSocket);
				m_ServerState = ServerState::Listening;
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

bool ModuleNetworkingServer::isRunning() const
{
	return m_ServerState != ServerState::Stopped;
}



// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (m_ServerState != ServerState::Stopped)
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

			flagServerDisconnect = true;
			m_ServerState = ServerState::Stopped;
			App->modScreen->swapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == m_ListeningSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	m_ConnectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, byte * data)
{
	// Set the client name of the corresponding connected socket proxy
	for (ModuleNetworkingServer::ConnectedSocket &connectedSocket : m_ConnectedSockets)
	{
		if (connectedSocket.socket == socket)
		{
			connectedSocket.client_name = (const char*)data;
			LOG("Received client name from client '%s'", connectedSocket.client_name.c_str());
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