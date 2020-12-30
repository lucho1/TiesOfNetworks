#include "ModuleNetworkingClient.h"


// --- ModuleNetworkingClient public methods ---
void ModuleNetworkingClient::SetServerAddress(const char * pServerAddress, uint16 pServerPort)
{
	m_ServerAddressStr = pServerAddress;
	m_ServerPort = pServerPort;
}

void ModuleNetworkingClient::SetPlayerInfo(const char * pPlayerName, uint8 pSpaceshipType)
{
	m_PlayerName = pPlayerName;
	m_SpaceshipType = pSpaceshipType;
}



// --- ModuleNetworking virtual methods ---
void ModuleNetworkingClient::OnStart()
{
	if (!CreateSocket())
		return;

	if (!BindSocketToPort(0))
	{
		Disconnect();
		return;
	}

	// Create remote address
	m_ServerAddress = {};
	m_ServerAddress.sin_family = AF_INET;
	m_ServerAddress.sin_port = htons(m_ServerPort);
	int res = inet_pton(AF_INET, m_ServerAddressStr.c_str(), &m_ServerAddress.sin_addr);

	if (res == SOCKET_ERROR)
	{
		ReportError("ModuleNetworkingClient::startClient() - inet_pton");
		Disconnect();
		return;
	}

	m_State = ClientState::CONNECTING;
	m_InputDataFront = 0;
	m_InputDataBack = 0;
	m_SecondsSinceLastHello = 9999.0f;
	m_SecondsSinceLastInputDelivery = 0.0f;
	m_LastPingSent = 0.0f;
	m_LastPingReceived = 0.0f;
}

void ModuleNetworkingClient::OnGUI()
{
	if (m_State == ClientState::STOPPED)
		return;

	if (ImGui::CollapsingHeader("ModuleNetworkingClient", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_State == ClientState::CONNECTING)
			ImGui::Text("Connecting to server...");
		else if (m_State == ClientState::CONNECTED)
		{
			ImGui::Text("Connected to server");

			// Player Info
			ImGui::Separator();

			ImGui::Text("Player info:");
			ImGui::Text(" - Id: %u", m_PlayerId);
			ImGui::Text(" - Name: %s", m_PlayerName.c_str());

			// Spaceship Info
			ImGui::Separator();

			ImGui::Text("Spaceship info:");
			ImGui::Text(" - Type: %u", m_SpaceshipType);
			ImGui::Text(" - Network id: %u", m_NetworkId);

			vec2 playerPosition = {};
			GameObject *playerGameObject = App->modLinkingContext->GetNetworkGameObject(m_NetworkId);

			if (playerGameObject != nullptr)
				playerPosition = playerGameObject->position;
			
			ImGui::Text(" - Coordinates: (%f, %f)", playerPosition.x, playerPosition.y);

			// Connection Info
			ImGui::Separator();

			ImGui::Text("Connection checking info:");
			ImGui::Text(" - Ping interval (s): %f", PING_INTERVAL_SECONDS);
			ImGui::Text(" - Disconnection timeout (s): %f", DISCONNECT_TIMEOUT_SECONDS);

			ImGui::Separator();

			ImGui::Text("Input:");
			ImGui::InputFloat("Delivery interval (s)", &m_InputDeliveryIntervalSeconds, 0.01f, 0.1f, 4);
			ImGui::Text("Input Data Front: %i", m_InputDataFront);
			ImGui::Text("Input Data Back: %i", m_InputDataBack);
			ImGui::Text("Last Received Input SeqNum: %i", m_LastReceivedInputNum);
		}
	}
}

void ModuleNetworkingClient::OnPacketReceived(const InputMemoryStream &packet, const sockaddr_in &fromAddress)
{
	// TODO(you): UDP virtual connection lab session
	uint32 protoId;
	packet >> protoId;
	if (protoId != PROTOCOL_ID)
		return;

	ServerMessage message;
	packet >> message;

	if (m_State == ClientState::CONNECTING)
	{
		if (message == ServerMessage::WELCOME)
		{
			packet >> m_PlayerId;
			packet >> m_NetworkId;

			CONSOLE_INFO_LOG("ModuleNetworkingClient::onPacketReceived() - Welcome from server");
			m_State = ClientState::CONNECTED;
			m_LastPingReceived = Time.time;
		}
		else if (message == ServerMessage::UNWELCOME)
		{
			CONSOLE_WARN_LOG("ModuleNetworkingClient::onPacketReceived() - Unwelcome from server :-(");
			Disconnect();
		}
	}
	else if (m_State == ClientState::CONNECTED)
	{
		switch (message)
		{
			case ServerMessage::PING:
			{
				m_LastPingReceived = Time.time;
				break;
			}
			case ServerMessage::REPLICATION:
			{
				// TODO(you): World state replication lab session
				packet >> m_LastReceivedInputNum;
				m_InputDataFront = m_LastReceivedInputNum;
				
				m_RepManager.Read(packet);
				break;
			}
			default:
				break;
		}
		// TODO(you): Reliability on top of UDP lab session
	}
}

void ModuleNetworkingClient::OnUpdate()
{
	if (m_State == ClientState::STOPPED)
		return;

	// TODO(you): UDP virtual connection lab session
	if (m_State == ClientState::CONNECTING)
	{
		m_SecondsSinceLastHello += Time.deltaTime;

		if (m_SecondsSinceLastHello > 0.1f)
		{
			m_SecondsSinceLastHello = 0.0f;

			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::HELLO;
			packet << m_PlayerName;
			packet << m_SpaceshipType;

			SendPacket(packet, m_ServerAddress);
		}
	}
	else if (m_State == ClientState::CONNECTED)
	{
		// TODO(you): UDP virtual connection lab session

		if (Time.time - m_LastPingSent >= PING_INTERVAL_SECONDS) {
			m_LastPingSent = Time.time;

			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::PING;

			SendPacket(packet, m_ServerAddress);
		}

		if (Time.time - m_LastPingReceived >= DISCONNECT_TIMEOUT_SECONDS) {
			CONSOLE_ERROR_LOG("Server Error: Connection Timeout");
			Disconnect();
		}

		// Process more inputs if there's space
		if (m_InputDataBack - m_InputDataFront < ArrayCount(m_InputData))
		{
			// Pack current input
			uint32 currentInputData = m_InputDataBack++;
			InputPacketData &inputPacketData = m_InputData[currentInputData % ArrayCount(m_InputData)];

			inputPacketData.sequenceNumber = currentInputData;
			inputPacketData.horizontalAxis = Input.horizontalAxis;
			inputPacketData.verticalAxis = Input.verticalAxis;
			inputPacketData.buttonBits = PackInputControllerButtons(Input);
		}

		m_SecondsSinceLastInputDelivery += Time.deltaTime;

		// Input delivery interval timed out: create a new input packet
		if (m_SecondsSinceLastInputDelivery > m_InputDeliveryIntervalSeconds)
		{
			m_SecondsSinceLastInputDelivery = 0.0f;

			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::INPUT;

			// TODO(you): Reliability on top of UDP lab session
			for (uint32 i = m_InputDataFront; i < m_InputDataBack; ++i)
			{
				InputPacketData &inputPacketData = m_InputData[i % ArrayCount(m_InputData)];

				packet << inputPacketData.sequenceNumber;
				packet << inputPacketData.horizontalAxis;
				packet << inputPacketData.verticalAxis;
				packet << inputPacketData.buttonBits;
			}

			// Clear the queue
			SendPacket(packet, m_ServerAddress);
		}

		// TODO(you): Latency management lab session
		// Update camera for player
		GameObject *playerGameObject = App->modLinkingContext->GetNetworkGameObject(m_NetworkId);
		if (playerGameObject != nullptr)
			App->modRender->cameraPosition = playerGameObject->position;
		else
		{
			// This means that the player has been destroyed (e.g. killed)
		}
	}
}


void ModuleNetworkingClient::OnDisconnect()
{
	m_State = ClientState::STOPPED;

	GameObject *networkGameObjects[MAX_NETWORK_OBJECTS] = {};
	uint16 networkGameObjectsCount;
	App->modLinkingContext->GetNetworkGameObjects(networkGameObjects, &networkGameObjectsCount);
	App->modLinkingContext->Clear();

	for (uint32 i = 0; i < networkGameObjectsCount; ++i)
		Destroy(networkGameObjects[i]);

	App->modRender->cameraPosition = {};
}