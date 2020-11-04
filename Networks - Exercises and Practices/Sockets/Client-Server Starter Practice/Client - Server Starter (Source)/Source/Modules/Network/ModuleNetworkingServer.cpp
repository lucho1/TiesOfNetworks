#include "ModuleNetworkingServer.h"

// -- Delivery by Lucho Suaya and Sergi Parra --



// ---------------------- Virtual functions of Modules -----------------------
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

	APPCONSOLE_INFO_LOG("SERVER STARTED IN PORT %i", port);

	// --- ---
	return true;
}

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

		for (ConnectedSocket &connectedSocket : m_ConnectedSockets)
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

		// Input message & Send button
		static char buffer[250]{ "Write a Server Message" };
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

		ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::Separator();
		if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);

		if (ImGui::InputText("##ConsoleInputText", buffer, 250, flags) || ImGui::Button("Send"))
		{
			OutputMemoryStream packet;
			SetupPacket(packet, SERVER_MESSAGE::SERVER_WARN, std::string(buffer), 0, Colors::ConsoleYellow);
			
			for (ConnectedSocket& client : m_ConnectedSockets)
				SendPacket(packet, client.socket); //TTT

			sprintf_s(buffer, "");
		}

		// Disconnect Button
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Disconnect"))
		{
			for (ConnectedSocket& s : m_ConnectedSockets)
				m_DisconnectedSockets.push_back(s.socket);

			m_ServerDisconnection = true;
			m_ServerState = ServerState::STOPPED;
			App->modScreen->SwapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}



// ------------------ ModuleNetworkingServer methods ------------------
bool ModuleNetworkingServer::IsRunning() const
{
	return m_ServerState != ServerState::STOPPED;
}

uint ModuleNetworkingServer::FindSocket(const SOCKET& socket)
{
	if (m_ConnectedSockets.empty())
		return -1;

	for (uint i = 0; i < m_ConnectedSockets.size(); ++i)
		if (m_ConnectedSockets[i].socket == socket)
			return i;

	return -1;
}

void ModuleNetworkingServer::SetupPacket(OutputMemoryStream& packet, SERVER_MESSAGE msg_type, std::string msg, uint src_id, const Color& msg_color)
{
	packet << (int)msg_type << msg << src_id << msg_color;
}

void ModuleNetworkingServer::ReadPacket(const InputMemoryStream& packet, CLIENT_MESSAGE& msg_type, std::string& msg, uint& src_id, Color& msg_color)
{
	int int_type = -1;
	packet >> int_type >> msg >> src_id >> msg_color;
	msg_type = (CLIENT_MESSAGE)int_type;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	// --- Find Socket in Vector ---
	uint s_index = FindSocket(socket);
	if (s_index == -1)
		return;

	// --- If found, read data & prepare response ---
	ConnectedSocket s = m_ConnectedSockets[s_index];
	OutputMemoryStream server_response;
	
	CLIENT_MESSAGE message_type;
	std::string message;
	uint source_id;
	Color message_color;

	ReadPacket(packet, message_type, message, source_id, message_color);

	// --- Decide what to do with packet according to its type ---
	switch (message_type)
	{
		case CLIENT_MESSAGE::CLIENT_CONNECTION:
		{
			s.client_name = message;
			APPCONSOLE_INFO_LOG("Connected Client '%s'", message.c_str());

			std::string msg = "[SERVER]: Client '" + message + "' (ID '#" + std::to_string(s.id) + "') Connected";
			SetupPacket(server_response, SERVER_MESSAGE::SERVER_INFO, msg.c_str(), s.id, Colors::ConsoleBlue); //TTT

			for (ConnectedSocket& client : m_ConnectedSockets)
				SendPacket(server_response, client.socket);

			break;
		}
		case CLIENT_MESSAGE::CLIENT_TEXT:
		{
			SetupPacket(server_response, SERVER_MESSAGE::CLIENT_TEXT, message, s.id, message_color);
			for (ConnectedSocket& client : m_ConnectedSockets)
				SendPacket(server_response, client.socket);

			break;
		}
		case CLIENT_MESSAGE::CLIENT_PRIVATE_TEXT:
		{
			for (ConnectedSocket& client : m_ConnectedSockets)
			{
				if (client.id == source_id)
				{
					SetupPacket(server_response, SERVER_MESSAGE::CLIENT_PRIVATE_TEXT, message, s.id, message_color);
					SendPacket(server_response, client.socket);
				}
			}

			break;
		}
		case CLIENT_MESSAGE::CLIENT_COMMAND:
		{
			// First read the command and then make the response. For instance:
			// Client reads the first char to decide sending CLIENT_COMMAND ("/")
			// Client also reads the COMMAND_NAME ("whisper") and COMMAND_ARG ("user")
			// if the command can solve the command ("help", "list" (?), ...) solve it there.
			// Otherwise, in here place we make a switch() to decide what to do with the command sent

			//if (COMMAND_NAME == "change_name")
			//{
			//	We could make a "WARN" so the text is displayed yellow
			//}

			break;
		}
		case CLIENT_MESSAGE::CLIENT_DISCONNECTION:
		{
			std::string msg = "[SERVER]: Client '" + s.client_name + "' (ID '#" + std::to_string(s.id) + "') Disconnected";

			SetupPacket(server_response, SERVER_MESSAGE::SERVER_INFO, msg.c_str(), s.id, Colors::ConsoleYellow); //TTT
			for (ConnectedSocket& client : m_ConnectedSockets)
				SendPacket(server_response, client.socket);

			break;
		}
	}
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in& socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	m_ConnectedSockets.push_back(connectedSocket);
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

			APPCONSOLE_INFO_LOG("Disconnected client '%s' with address %s and ID: %d", connectedSocket.client_name.c_str(), add.c_str(), connectedSocket.socket);
			m_ConnectedSockets.erase(it);
			break;
		}
	}
}