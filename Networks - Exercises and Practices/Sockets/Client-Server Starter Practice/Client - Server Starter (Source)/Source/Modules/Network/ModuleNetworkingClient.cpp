#include "ModuleNetworkingClient.h"

// -- Delivery by Lucho Suaya and Sergi Parra --



ModuleNetworkingClient::ModuleNetworkingClient() : ModuleNetworking() {
	// mTODO Sergi: Add commands to map here
	m_UserCommands["whisper"] = CLIENT_COMMANDS::COMMAND_WHISPER;
	m_UserCommands["w"] = CLIENT_COMMANDS::COMMAND_WHISPER;
	m_UserCommands["logout"] = CLIENT_COMMANDS::COMMAND_LOGOUT;
	m_UserCommands["nick"] = CLIENT_COMMANDS::COMMAND_CHANGE_NICK;
	m_UserCommands["help"] = CLIENT_COMMANDS::COMMAND_HELP;
	m_UserCommands["h"] = CLIENT_COMMANDS::COMMAND_HELP;
	m_UserCommands["kick"] = CLIENT_COMMANDS::COMMAND_KICK;
	m_UserCommands["k"] = CLIENT_COMMANDS::COMMAND_KICK;


	// mTODO Sergi: Add descriptions to commands here
	m_UserCmdDescriptions[CLIENT_COMMANDS::COMMAND_HELP] = "/help or /h - Displays help on command(s), syntax is \"/help (command)\"";
	m_UserCmdDescriptions[CLIENT_COMMANDS::COMMAND_WHISPER] = "/whisper or /w - Message someone privately, syntax is \"/whisper [username] [message]\"";
	m_UserCmdDescriptions[CLIENT_COMMANDS::COMMAND_LOGOUT] = "/logout - Disconnects from chat";
	m_UserCmdDescriptions[CLIENT_COMMANDS::COMMAND_CHANGE_NICK] = "/nick - Changes your name in the chat, syntax is \"/nick [new_username]\"";
	m_UserCmdDescriptions[CLIENT_COMMANDS::COMMAND_KICK] = "/kick or /k - Kicks user out of server, syntax is \"/kick [username]\"";
}

// ------------------ ModuleNetworkingClient public methods ------------------
bool  ModuleNetworkingClient::Start(const char * serverAddressStr, int serverPort, const char * clientName)
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

		sockaddr_in server_sockaddrin;
		server_sockaddrin.sin_family = AF_INET;
		server_sockaddrin.sin_port = htons(serverPort);
		inet_pton(AF_INET, serverAddressStr, &server_sockaddrin.sin_addr);
	
		if (connect(m_Socket, (sockaddr*)&server_sockaddrin, sizeof(server_sockaddrin)) != SOCKET_ERROR)
		{
			AddSocket(m_Socket);
			m_ClientState = ClientState::START; // If everything was ok... change the state
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

bool ModuleNetworkingClient::IsRunning() const
{
	return m_ClientState != ClientState::STOPPED;
}

// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingClient::DrawUI_SendButton() // A bit hardcoded but visually better
{
	ImGui::SameLine();
	return ImGui::Button("Send");
}

void ModuleNetworkingClient::ParseMessage(const std::string& buffer) {
	OutputMemoryStream packet;

	if (buffer[0] == '/') { 		// Is a command
		if (buffer.size() == 1)
			return; //nothing to do here, it's an empty command

		std::size_t pos = buffer.find_first_of(' ');
		std::string command;

		if (pos == std::string::npos)
			command = buffer.substr(1);
		else
			command = buffer.substr(1, pos - 1);

		CLIENT_COMMANDS m_command;
		try {
			m_command = m_UserCommands.at(command);
		} catch (const std::out_of_range & e) {
			m_command = CLIENT_COMMANDS::COMMAND_INVALID;
		}

		switch (m_command) {
		case CLIENT_COMMANDS::COMMAND_WHISPER:
			{
			std::string dst_user, msg;

			std::size_t start_pos = buffer.find_first_not_of(' ', pos);
			if (start_pos == std::string::npos) {
				std::string warning = "No username, use /whisper username [message]";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}

			pos = buffer.find_first_of(' ', start_pos);
			dst_user = buffer.substr(start_pos, pos - start_pos);

			start_pos = buffer.find_first_not_of(' ', pos);
			if (start_pos == std::string::npos) {
				std::string warning = "No message, use /whisper username [message]";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}

			msg = buffer.substr(start_pos);

			if (dst_user == m_ClientName) {
				std::string warning = "No need for a chat to message yourself!";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}

			uint dst_id;
			try {
				dst_id = m_ConnectedUsers.at(dst_user);
			}
			catch (const std::out_of_range & e) {
				std::string warning = "The user '" + dst_user + "' is not in this chat!";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}

			packet << CLIENT_MESSAGE::CLIENT_PRIVATE_TEXT << dst_id << msg;
			SendPacket(packet, m_Socket);

			std::string displayed_message = "    Whisper to <" + dst_user + ">: " + msg;
			App->modUI->PrintMessageInConsole(displayed_message.c_str());

			break;
			} //COMMAND_WHISPER

		case CLIENT_COMMANDS::COMMAND_LOGOUT:
			{
			OutputMemoryStream packet;
			packet << CLIENT_MESSAGE::CLIENT_DISCONNECTION;
			SendPacket(packet, m_Socket);

			m_ConnectedUsers.clear();
			m_DisconnectedSockets.push_back(m_Socket);
			break;
			} //COMMAND_LOGOUT

		case CLIENT_COMMANDS::COMMAND_CHANGE_NICK:
			{
			std::size_t start_pos = buffer.find_first_not_of(' ', pos);
			if (start_pos == std::string::npos) {
				std::string warning = "No username, use /nick [username]";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}

			pos = buffer.find_first_of(' ', start_pos);
			std::string new_nickname = buffer.substr(start_pos, pos - start_pos);
			if (new_nickname == m_ClientName) {
				std::string warning = "No need to change your username, it already is '" + m_ClientName +"'!";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}

			OutputMemoryStream packet;
			packet << CLIENT_MESSAGE::CLIENT_COMMAND << m_command << new_nickname;
			SendPacket(packet, m_Socket);

			break;
			} //COMMAND_CHANGE_NICK 

		case CLIENT_COMMANDS::COMMAND_HELP:
		{
			std::size_t start_pos = buffer.find_first_not_of(' ', pos);
			if (start_pos != std::string::npos) {
				pos = buffer.find_first_of(' ', start_pos);
				std::string help_command = buffer.substr(start_pos, pos - start_pos);
				CLIENT_COMMANDS help_command_enum;

				try {
					help_command_enum = m_UserCommands.at(help_command);
				}
				catch (const std::out_of_range & e) {
					std::string warning = "Command '" + help_command + "' does not exist, type /help for a list of commands.";
					APPCONSOLE_WARN_LOG(warning.c_str());
					break;
				}

				std::string help_message;
				try {
					help_message = m_UserCmdDescriptions.at(help_command_enum);
				}
				catch (const std::out_of_range & e) {
					break; //nothing to do, this command has no description
				}

				App->modUI->PrintMessageInConsole(help_message.c_str(), Colors::ConsoleBlue);
			}
			else {
				for (auto cmd_description : m_UserCmdDescriptions)
					App->modUI->PrintMessageInConsole(cmd_description.second.c_str(), Colors::ConsoleBlue);
			}
			break;
			} // COMMAND_HELP
			
		case CLIENT_COMMANDS::COMMAND_KICK:
			{
			std::size_t start_pos = buffer.find_first_not_of(' ', pos);
			if (start_pos == std::string::npos) {
				std::string warning = "No username, use /kick [username]";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break;
			}
			pos = buffer.find_first_of(' ', start_pos);
			std::string kicked_user = buffer.substr(start_pos, pos - start_pos);

			int user_id = -1;
			try {
				user_id = m_ConnectedUsers.at(kicked_user);
			} catch (const std::out_of_range & e) {
				std::string warning = "User '" + kicked_user + "' does not exist.";
				APPCONSOLE_WARN_LOG(warning.c_str());
				break; //nothing to do, this command has no description
			}

			OutputMemoryStream packet;
			packet << CLIENT_MESSAGE::CLIENT_COMMAND << m_command << user_id;
			SendPacket(packet, m_Socket);

			break;
			} //COMMAND_KICK

		case CLIENT_COMMANDS::COMMAND_INVALID:
			{
			std::string warning = "The command '" + command + "' does not exist!";
			APPCONSOLE_WARN_LOG(warning.c_str());

			break;
			} //COMMAND_INVALID
		} //switch (m_command)



	}
	else {
		// Regular message
		packet << CLIENT_MESSAGE::CLIENT_TEXT << std::string(buffer) << m_UserTextColor;
		SendPacket(packet, m_Socket);
	}

}

bool ModuleNetworkingClient::GUI()
{
	if (m_ClientState == ClientState::ONLINE)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("Welcome '%s' to the '%s' server!", m_ClientName.c_str(), m_ServerName.c_str());
		ImGui::Text("Server Address: %s", m_ServerAddressStr.c_str());

		// Input message & Send button
		static char buffer[250]{ "Write a Message" };
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

		ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::Separator();
		if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);

		if (ImGui::InputText("##ConsoleInputText", buffer, 250, flags) || DrawUI_SendButton())
		{
			ParseMessage(buffer);
			sprintf_s(buffer, "");
		}

		ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueWheel;
		ImGui::SameLine();
		ImGui::ColorEdit4("Text Color", &m_UserTextColor, color_flags);

		// Add functionality for Private Text & Commands here


		// Disconnect Button
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine(); ImGui::Separator();
		if (ImGui::Button("Disconnect"))
		{
			// Notify Disconnection
			OutputMemoryStream packet;
			packet << CLIENT_MESSAGE::CLIENT_DISCONNECTION;
			SendPacket(packet, m_Socket);

			m_ConnectedUsers.clear();
			m_DisconnectedSockets.push_back(m_Socket);
		}

		ImGui::End();
	}

	return true;
}

bool ModuleNetworkingClient::Update()
{
	switch (m_ClientState) {
	case ClientState::START: 
		{
		OutputMemoryStream packet;
		packet << CLIENT_MESSAGE::CLIENT_CONNECTION << m_ClientName;

		SendPacket(packet, m_Socket);
		m_ClientState = ClientState::LOGGING;
		break;
		}

	default:
		break;
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	// -- Read Data --
	SERVER_MESSAGE message_type;
	packet >> message_type;

	switch (message_type)
	{
		case SERVER_MESSAGE::CLIENT_TEXT:
		{
			std::string username, message;
			Color message_color;

			packet >> username >> message >> message_color;
			std::string displayed_message = "<" + username + ">: " + message;
			App->modUI->PrintMessageInConsole(displayed_message.c_str(), message_color);
			break;
		}
		case SERVER_MESSAGE::CLIENT_PRIVATE_TEXT:
		{
			std::string username, message;

			packet >> username >> message;
			std::string displayed_message = "    Whisper from <" + username + ">: " + message;
			App->modUI->PrintMessageInConsole(displayed_message.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_USER_CONNECTION:
		{
			std::string username, message;
			uint user_id;
			packet >> username >> user_id >> message;

			message = "<Server>: " + message;
			m_ConnectedUsers[username] = user_id;

			APPCONSOLE_INFO_LOG(message.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_USER_DISCONNECTION:
		{
			std::string username, message;
			packet >> username >> message;

			message = "<Server>: " + message;
			auto user = m_ConnectedUsers.find(username);
			if (user != m_ConnectedUsers.end())
				m_ConnectedUsers.erase(user);

			APPCONSOLE_INFO_LOG(message.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_INFO:
		{
			std::string message;
			packet >> message;

			message = "<Server>: " + message;
			APPCONSOLE_INFO_LOG(message.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_WARN:	
		{
			std::string message;
			packet >> message;

			message = "<Server>: " + message;
			APPCONSOLE_WARN_LOG(message.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_ERROR:
		{
			std::string message;
			packet >> message;

			message = "<Server>: " + message;
			APPCONSOLE_ERROR_LOG(message.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_REJECTION:
		{
			packet >> reject_message;

			reject_message = "<Server>: " + reject_message;
			m_DisconnectedSockets.push_back(m_Socket);
			m_ClientState = ClientState::REJECTED;
			break;
		}case SERVER_MESSAGE::SERVER_FORCE_DISCONNECTION:
		{
			packet >> reject_message;

			reject_message = "<Server>: " + reject_message;

			// Notify Disconnection
			OutputMemoryStream packet;
			packet << CLIENT_MESSAGE::CLIENT_DISCONNECTION;
			SendPacket(packet, m_Socket);

			m_ConnectedUsers.clear();
			m_DisconnectedSockets.push_back(m_Socket);
			m_ClientState = ClientState::REJECTED;
			break;
		}
		case SERVER_MESSAGE::SERVER_WELCOME:
		{
			m_ClientState = ClientState::ONLINE;
			std::string welcome_msg;
			packet >> m_ServerName >> welcome_msg >> m_ClientID >> m_ConnectedUsers;
			welcome_msg = "<Server>: " + welcome_msg;
			APPCONSOLE_INFO_LOG(welcome_msg.c_str());
			break;
		}
		case SERVER_MESSAGE::SERVER_USER_NEW_NICK:
		{
			std::string old_username, new_username;
			uint user_id;
			packet >> old_username >> new_username >> user_id;

			if (old_username == m_ClientName) {
				m_ClientName = new_username;
			}
			else {
				auto position = m_ConnectedUsers.find(old_username);
				if (position != m_ConnectedUsers.end())
					m_ConnectedUsers.erase(position);
				m_ConnectedUsers[new_username] = user_id;
			}

			std::string message = "<Server>: User '" + old_username + "' is now known as '" + new_username + "'";
			APPCONSOLE_INFO_LOG(message.c_str());

			break;
		}
		case SERVER_MESSAGE::SERVER_DISCONNECTION:	m_ServerDisconnection = true;								break;

	}
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	// Disconnect Socket
	//m_DisconnectedSockets.push_back(m_Socket);
	
	if (shutdown(socket, 2) == SOCKET_ERROR)
		ReportErrorAndClose(socket, { "[CLIENT]: Error shuting down the socket of client '" + m_ClientName + "'"}, m_ClientName + " CLIENT", "ModuleNetworkingClient::onSocketDisconnected()");
	else if (closesocket(socket) == SOCKET_ERROR)
		ReportError(std::string("[CLIENT]: Error Closing socket from '" + m_ClientName + "' Client on function ModuleNetworkingClient::onSocketDisconnected()").c_str());

	// Clear Client Console & Log/Change State
	App->modUI->ClearConsoleMessages();
	if (m_ClientState == ClientState::REJECTED)
		APPCONSOLE_ERROR_LOG(reject_message.c_str());
	else
		APPCONSOLE_INFO_LOG("Disconnected Client '%s' from Server '%s' (IP: %s)", m_ClientName.c_str(), m_ServerName.c_str(), m_ServerAddressStr.c_str());

	m_ClientState = ClientState::STOPPED;

	// If server was Disconnected, notify
	if (m_ServerDisconnection)
	{
		APPCONSOLE_WARN_LOG("[SERVER]: Server '%s' with IP: %s was Disconnected", m_ServerName.c_str(), m_ServerAddressStr.c_str());
		m_ServerDisconnection = false;
	}
}