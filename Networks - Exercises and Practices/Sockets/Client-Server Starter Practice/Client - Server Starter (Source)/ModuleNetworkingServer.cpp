#include "ModuleNetworkingServer.h"


// ------------------ ModuleNetworkingServer public methods ------------------
bool ModuleNetworkingServer::start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// - Set address reuse
	// - Bind the socket to a local interface
	// - Enter in listen mode
	// - Add the listenSocket to the managed list of sockets using addSocket()

	m_ServerState = ServerState::Listening;
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

		for (auto &connectedSocket : m_ConnectedSockets)
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
	// Set the player name of the corresponding connected socket proxy
	for (auto &connectedSocket : m_ConnectedSockets)
	{
		if (connectedSocket.socket == socket)
		{
			connectedSocket.client_name = (const char *)data;
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
			m_ConnectedSockets.erase(it);
			break;
		}
	}
}