#include "ModuleNetworkingClient.h"

// -- Delivery by Lucho Suaya and Sergi Parra --

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
		m_ServerAddress.sin_family = AF_INET;
		m_ServerAddress.sin_port = htons(serverPort);
		inet_pton(AF_INET, serverAddressStr, &m_ServerAddress.sin_addr);
	
		if (connect(m_Socket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) != SOCKET_ERROR)
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

void ModuleNetworkingClient::SetupPacket(OutputMemoryStream& packet, CLIENT_MESSAGE msg_type, std::string msg, const Color& msg_color)
{
	packet << (int)msg_type << msg << msg_color;
}

void ModuleNetworkingClient::ReadPacket(const InputMemoryStream& packet, SERVER_MESSAGE& msg_type, std::string& msg, uint& src_id, Color& msg_color)
{
	int int_type = -1;
	packet >> int_type >> msg >> src_id >> msg_color;
	msg_type = (SERVER_MESSAGE)int_type;
}



// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingClient::DrawUI_SendButton() // A bit hardcoded but visually better
{
	ImGui::SameLine();
	return ImGui::Button("Send");
}

bool ModuleNetworkingClient::GUI()
{
	if (m_ClientState != ClientState::STOPPED)
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
			OutputMemoryStream packet;
			SetupPacket(packet, CLIENT_MESSAGE::CLIENT_TEXT, std::string(buffer), Colors::ConsoleGreen);
			SendPacket(packet, m_Socket);
			sprintf_s(buffer, "");
		}

		// Add functionality for Private Text & Commands here


		// Disconnect Button
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine(); ImGui::Separator();
		if (ImGui::Button("Disconnect"))
		{
			// Notify Disconnection
			OutputMemoryStream packet;
			SetupPacket(packet, CLIENT_MESSAGE::CLIENT_DISCONNECTION, "", Colors::ConsoleBlue);
			SendPacket(packet, m_Socket);

			m_DisconnectedSockets.push_back(m_Socket);
			m_ClientState = ClientState::STOPPED;
			App->modScreen->SwapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}

bool ModuleNetworkingClient::Update()
{
	if (m_ClientState == ClientState::START)
	{
		OutputMemoryStream packet;
		SetupPacket(packet, CLIENT_MESSAGE::CLIENT_CONNECTION, m_ClientName, Colors::ConsoleBlue);
		packet << m_ServerAddressStr;

		SendPacket(packet, m_Socket);
		m_ClientState = ClientState::LOGGING;
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	// -- Read Data --
	SERVER_MESSAGE message_type;
	std::string message;
	uint source_id;
	Color message_color;

	ReadPacket(packet, message_type, message, source_id, message_color);

	switch (message_type)
	{
		case SERVER_MESSAGE::CLIENT_TEXT:
		{
			std::string displayed_message = "[#" + std::to_string(source_id) + std::string("]: ") + message;
			App->modUI->PrintMessageInConsole(displayed_message.c_str(), message_color);
			break;
		}
		case SERVER_MESSAGE::CLIENT_PRIVATE_TEXT:
		{
			std::string displayed_message = "- [PRIV] - [" + std::to_string(source_id) + std::string("]: ") + message;
			App->modUI->PrintMessageInConsole(displayed_message.c_str(), message_color);
			break;
		}
		case SERVER_MESSAGE::SERVER_INFO:			APPCONSOLE_INFO_LOG(message.c_str());						break;
		case SERVER_MESSAGE::SERVER_WARN:			APPCONSOLE_WARN_LOG(message.c_str());						break;
		case SERVER_MESSAGE::SERVER_ERROR:			APPCONSOLE_ERROR_LOG(message.c_str());						break;
		case SERVER_MESSAGE::SERVER_DISCONNECTION:	m_ServerDisconnection = true;								break;
		case SERVER_MESSAGE::SERVER_WELCOME:
		{
			packet >> m_ServerName;
			APPCONSOLE_INFO_LOG(message.c_str());
			break;
		}
	}
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	// Disconnect Socket
	m_DisconnectedSockets.push_back(m_Socket);
	
	if (shutdown(socket, 2) == SOCKET_ERROR)
		ReportErrorAndClose(socket, { "[CLIENT]: Error shuting down the socket of client '" + m_ClientName + "'"}, m_ClientName + " CLIENT", "ModuleNetworkingClient::onSocketDisconnected()");
	else if (closesocket(socket) == SOCKET_ERROR)
		ReportError(std::string("[CLIENT]: Error Closing socket from '" + m_ClientName + "' Client on function ModuleNetworkingClient::onSocketDisconnected()").c_str());

	// Clear Client Console & Log/Change State
	App->modUI->ClearConsoleMessages();
	m_ClientState = ClientState::STOPPED;
	APPCONSOLE_INFO_LOG("Disconnected Client '%s' from Server '%s' (IP: %s)", m_ClientName.c_str(), m_ServerName.c_str(), m_ServerAddressStr.c_str());

	// If server was Disconnected, notify
	if (m_ServerDisconnection)
	{
		APPCONSOLE_WARN_LOG("[SERVER]: Server '%s' with IP: %s was Disconnected", m_ServerName.c_str(), m_ServerAddressStr.c_str());
		m_ServerDisconnection = false;
	}
}