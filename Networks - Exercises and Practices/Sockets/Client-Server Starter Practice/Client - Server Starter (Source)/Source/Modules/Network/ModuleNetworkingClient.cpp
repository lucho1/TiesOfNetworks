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

void ModuleNetworkingClient::SetupPacket(OutputMemoryStream& packet, CLIENT_MESSAGE msg_type, std::string msg, uint src_id, const Color& msg_color)
{
	packet << (int)msg_type << msg << src_id << msg_color;
}



// ---------------------- Virtual functions of Modules -----------------------
bool ModuleNetworkingClient::Update()
{
	if (m_ClientState == ClientState::START)
	{
		OutputMemoryStream packet;
		SetupPacket(packet, CLIENT_MESSAGE::CLIENT_CONNECTION, "", 0, Colors::ConsoleBlue); //TTT
		SendPacket(packet, m_Socket);
		m_ClientState = ClientState::LOGGING;

		//if (SendPacket(packet, m_Socket))
		//	m_ClientState = ClientState::LOGGING;
		//else
		//{
		//	APPCONSOLE_ERROR_LOG(std::string("[CLIENT]: Error sending data to server '" + m_ServerAddressStr + "' from " + m_ClientName + " client on ModuleNetworkingClient::update()").c_str());
		//	Disconnect();
		//	m_ClientState = ClientState::STOPPED;
		//}
	}

	return true;
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

		ImGui::Text("'%s' connected to the server '%s'...", m_ClientName.c_str(), m_ServerAddressStr.c_str());

		// Input message & Send button
		static char buffer[250]{ "Write a Message" };

		ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::Separator();
		ImGui::InputText("##ConsoleInputText", buffer, 250);
		ImGui::SameLine();

		if (ImGui::Button("Send"))
		{
			OutputMemoryStream packet;
			SetupPacket(packet, CLIENT_MESSAGE::CLIENT_TEXT, std::string(buffer), 0, Colors::ConsoleGreen);
			if (!SendPacket(packet, m_Socket)) //TTT
				ReportError("COULDN'T SEND MESSAGE");
		}

		// Add functionality for Private Text & Commands here


		// Disconnect Button
		ImGui::SetCursorPos({ 145.0f, 650.0f });
		ImGui::NewLine();
		ImGui::Separator();
		if (ImGui::Button("Disconnect"))
		{
			m_DisconnectedSockets.push_back(m_Socket);
			m_ClientState = ClientState::STOPPED;
			App->modScreen->SwapScreensWithTransition(App->modScreen->screenGame, App->modScreen->screenMainMenu);
		}

		ImGui::End();
	}

	return true;
}



// ----------------- Virtual functions of ModuleNetworking -------------------
void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	int a = 0;
	SERVER_MESSAGE message_type;
	std::string msg;
	packet >> a >> msg;
	message_type = (SERVER_MESSAGE)a;

	switch (message_type)
	{
		case SERVER_MESSAGE::CLIENT_TEXT:
		{
			int src_id;
			Color msg_color;
			packet >> src_id >> msg_color;

			std::string displayed_message = "[" + src_id + std::string("]: ") + msg.c_str();
			App->modUI->PrintMessageInConsole(displayed_message.c_str(), msg_color);

			break;
		}
		case SERVER_MESSAGE::CLIENT_PRIVATE_TEXT:
		{
			int src_id;
			Color msg_color;
			packet >> src_id >> msg_color;

			std::string displayed_message = "- [PRIV] - [" + src_id + std::string("]: ") + msg;
			App->modUI->PrintMessageInConsole(displayed_message.c_str(), msg_color);

			break;
		}
		case SERVER_MESSAGE::SERVER_INFO:	APPCONSOLE_INFO_LOG(msg.c_str());	break;
		case SERVER_MESSAGE::SERVER_WARN:	APPCONSOLE_WARN_LOG(msg.c_str());	break;
		case SERVER_MESSAGE::SERVER_ERROR:	APPCONSOLE_ERROR_LOG(msg.c_str());	break;
	}
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	OutputMemoryStream packet;
	SetupPacket(packet, CLIENT_MESSAGE::CLIENT_DISCONNECTION, "", 0, Colors::ConsoleBlue); //TTT
	SendPacket(packet, m_Socket);
	m_DisconnectedSockets.push_back(m_Socket);
	
	if (shutdown(socket, 2) == SOCKET_ERROR)
		ReportErrorAndClose(socket, { "[CLIENT]: Error shuting down the socket of client '" + m_ClientName + "'"}, m_ClientName + " CLIENT", "ModuleNetworkingClient::onSocketDisconnected()");
	else if (closesocket(socket) == SOCKET_ERROR)
		ReportError(std::string("[CLIENT]: Error Closing socket from '" + m_ClientName + "' Client on function ModuleNetworkingClient::onSocketDisconnected()").c_str());

	App->modUI->ClearConsoleMessages();
	m_ClientState = ClientState::STOPPED;
	APPCONSOLE_INFO_LOG("Socket at '%s' Disconnected from Server '%s'", m_ClientName.c_str(), m_ServerAddressStr.c_str());
}