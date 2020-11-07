#include "ModuleNetworkingServer.h"
#include <iomanip>

// -- Delivery by Lucho Suaya and Sergi Parra --



// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingServer::Start(int port, const char* serverName)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// - Set address reuse
	// - Bind the socket to a local interface
	// - Enter in listen mode
	// - Add the listenSocket to the managed list of sockets using addSocket()

	// --- ---
	m_ServerName = serverName;
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

	APPCONSOLE_INFO_LOG("SERVER '%s' STARTED IN PORT %i", m_ServerName.c_str(), port);

	// --- ---
	return true;
}

bool ModuleNetworkingServer::GUI()
{
	if (m_ServerState != ServerState::STOPPED)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		std::string winTitle = m_ServerName + " Server Window";
		ImGui::Begin(winTitle.c_str());
		
		// -- Header & Title --
		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		std::string serverName = "****** " + m_ServerName + " Server******";

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
		ImGui::SetWindowFontScale(1.2f);
		
		float x = serverName.size();
		float xPos = ImGui::GetWindowSize().x / 2.0f - 145.0f;
		float originalCursor = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(xPos);


		ImGui::Text("****** %s Server******", m_ServerName.c_str());
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale(1.0f);

		// -- Information on Connections --
		ImGui::NewLine(); /*ImGui::NewLine(); ImGui::NewLine();*/
		//ImGui::Text("Server Address: %s", m_ServerAddress.c_str()); //Sergi: We would need a connection anyway, no point
		ImGui::Text("List of connected sockets:");
		for (const auto& connectedSocket : m_ConnectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.second.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.second.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.second.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.second.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.second.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.second.address.sin_port));

			std::string str = connectedSocket.second.client_name;
			ImGui::Text("Client name: %s - ID: #%i", connectedSocket.second.client_name.c_str(), connectedSocket.first);
		}

		// -- Input message & Send button --
		static char buffer[250]{ "Write a Server Message" };
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

		ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::Separator();
		if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);

		if (ImGui::InputText("##ConsoleInputText", buffer, 250, flags) || ImGui::Button("Send"))
		{
			OutputMemoryStream packet;
			packet << SERVER_MESSAGE::SERVER_WARN << std::string(buffer) << Colors::ConsoleYellow;
			
			for (const auto& client : m_ConnectedSockets)
				SendPacket(packet, client.second.socket);

			sprintf_s(buffer, "");
		}

		// -- Disconnect Button --
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Disconnect"))
		{
			OutputMemoryStream packet;
			packet << SERVER_MESSAGE::SERVER_DISCONNECTION;

			for (const auto& s : m_ConnectedSockets)
			{
				SendPacket(packet, s.second.socket);
				m_DisconnectedSockets.push_back(s.second.socket);
			}

			m_ServerDisconnection = true;
			m_ServerState = ServerState::STOPPED;
			App->modScreen->SwapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}



// ------------------ ModuleNetworkingServer methods ------------------
uint ModuleNetworkingServer::GetSocketIndex(const SOCKET& socket)
{
	if (m_ConnectedSockets.empty())
		return -1;

	for (const auto& conn_sockets : m_ConnectedSockets)
		if (conn_sockets.second.socket == socket)
			return conn_sockets.first;

	return -1;
}

const std::pair<std::string, uint> ModuleNetworkingServer::GetUserFromID(uint ID)
{
	if (m_ConnectedNicknames.empty())
		return { "NULL", -1 };

	for (const auto& connected_users : m_ConnectedNicknames)
		if (connected_users.second == ID)
			return connected_users;

	return { "NULL", -1 };
}

const std::pair<std::string, uint> ModuleNetworkingServer::GetNextUser(uint current_userID)
{
	if (m_ConnectedNicknames.empty())
		return { "NULL",  -1 };

	auto it = m_ConnectedNicknames.begin();
	if (current_userID == 0)
		return *it;

	for (it; it != m_ConnectedNicknames.end(); ++it)
	{
		if ((*it).second == current_userID)
		{
			if (it++ != m_ConnectedNicknames.end())
				return (*it++);
			else
				return *m_ConnectedNicknames.begin();
		}
	}

	return { "NULL",  -1 };
}


// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	// --- Find Socket in Vector ---
	uint s_index = GetSocketIndex(socket);
	if (s_index == -1)
		return;

	// --- If found, read data & prepare response ---
	CLIENT_MESSAGE message_type;
	packet >> message_type;

	ConnectedSocket& connected_socket = m_ConnectedSockets[s_index];
	OutputMemoryStream server_response;

	// --- Decide what to do with packet according to its type ---
	switch (message_type)
	{
		case CLIENT_MESSAGE::CLIENT_CONNECTION:
		{
			std::string username;
			packet >> username;
			connected_socket.client_name = username;
			bool n_username = false;

			try {
				m_ConnectedNicknames.at(username);
			}
			catch (const std::out_of_range & e) {
				n_username = true;
			}

			if (!n_username || username == "") { // Disconnect the client because name already exists or invalid
				server_response << SERVER_MESSAGE::SERVER_REJECTION;
				SendPacket(server_response, connected_socket.socket);
				break; // We don't need to do anything else so break out of the switch
			}
			// Implicit else

			
			// Send welcome to only client
			APPCONSOLE_INFO_LOG("Connected Client '%s (#%i)'", username, s_index);
			std::string welcome_msg = "Welcome to " + m_ServerName + "!";
			server_response << SERVER_MESSAGE::SERVER_WELCOME << m_ServerName << welcome_msg;
			server_response << s_index << m_ConnectedNicknames;
			SendPacket(server_response, connected_socket.socket);

			// Add nickname to list
			m_ConnectedNicknames[username] = s_index;

			// Send connection message to everyone
			server_response.Clear();
			welcome_msg = "'" + username + "' connected.";
			server_response << SERVER_MESSAGE::SERVER_USER_CONNECTION << username << s_index << welcome_msg;

			for (const auto& socket : m_ConnectedSockets) {
				if (socket.second.socket == connected_socket.socket)
					continue;
				SendPacket(server_response, socket.second.socket);
			}

			break;
		}
		case CLIENT_MESSAGE::CLIENT_TEXT:
		{
			// Get inputs
			std::string message;
			Color message_color;
			packet >> message >> message_color;

			server_response << SERVER_MESSAGE::CLIENT_TEXT << connected_socket.client_name << message << message_color;
			for (const auto& client : m_ConnectedSockets)
				SendPacket(server_response, client.second.socket);

			break;
		}
		case CLIENT_MESSAGE::CLIENT_PRIVATE_TEXT:
		{
			// Get inputs
			int dst_id;
			std::string message;
			packet >> dst_id >> message;

			if (m_ConnectedSockets.find(dst_id) != m_ConnectedSockets.end()) {
				const ConnectedSocket& destination = m_ConnectedSockets[dst_id];
				server_response << SERVER_MESSAGE::CLIENT_PRIVATE_TEXT << connected_socket.client_name << message;
				SendPacket(server_response, destination.socket);
			}
			else {
				server_response << SERVER_MESSAGE::SERVER_ERROR << "User does not exist!";
				SendPacket(server_response, connected_socket.socket);
			}
			break;
		}
		case CLIENT_MESSAGE::CLIENT_COMMAND:
		{
			CLIENT_COMMANDS command;
			packet >> command;
			
			switch (command) {
			case CLIENT_COMMANDS::COMMAND_CHANGE_NICK:
				{
				std::string new_username;
				packet >> new_username;
				bool n_username = false;

				try {
					m_ConnectedNicknames.at(new_username);
				}
				catch (const std::out_of_range & e) {
					n_username = true;
				}

				if (!n_username) {
					std::string warning = "The username '" + new_username + "' is taken!";
					server_response << SERVER_MESSAGE::SERVER_WARN << warning;
					SendPacket(server_response, connected_socket.socket);
				}
				else {
					std::string old_username = connected_socket.client_name;
					connected_socket.client_name = new_username;
					server_response << SERVER_MESSAGE::SERVER_USER_NEW_NICK << old_username << new_username << s_index;

					for (const auto& client : m_ConnectedSockets)
						SendPacket(server_response, client.second.socket);
				}

				break;
				}
			}

			break;
		}
		case CLIENT_MESSAGE::CLIENT_DISCONNECTION:
		{
			std::string msg = "'" + connected_socket.client_name + "' disconnected.";

			server_response << SERVER_MESSAGE::SERVER_USER_DISCONNECTION << connected_socket.client_name << msg;
			for (const auto& client : m_ConnectedSockets)
				SendPacket(server_response, client.second.socket);

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

	m_ConnectedSockets.insert({ m_ConnectedSockets.size(), connectedSocket });
}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	for (auto it = m_ConnectedSockets.begin(); it != m_ConnectedSockets.end(); ++it)
	{
		auto& iterator = *it;
		if (iterator.second.socket == socket)
		{
			auto nickname = m_ConnectedNicknames.find(iterator.second.client_name);
			if (nickname != m_ConnectedNicknames.end()) {
				m_ConnectedNicknames.erase(nickname);
			}

			ConnectedSocket connectedSocket = iterator.second;
			IN_ADDR inAddr = connectedSocket.address.sin_addr;
			std::string add = std::to_string(inAddr.S_un.S_un_b.s_b1) + "." + std::to_string(inAddr.S_un.S_un_b.s_b2) + "." + std::to_string(inAddr.S_un.S_un_b.s_b3)
				+ "." + std::to_string(inAddr.S_un.S_un_b.s_b4);// +":" + std::to_string(ntohs(connectedSocket.address.sin_port));

			APPCONSOLE_INFO_LOG("Disconnected client '%s (#%i)' with address '%s' and Socket ID: %d", connectedSocket.client_name.c_str(), iterator.first, add.c_str(), connectedSocket.socket);
			m_ConnectedSockets.erase(it);
			break;
		}
	}
}