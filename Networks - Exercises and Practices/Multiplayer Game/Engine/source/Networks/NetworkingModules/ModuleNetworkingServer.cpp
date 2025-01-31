#include "ModuleNetworkingServer.h"

// --- ModuleNetworking virtual methods ---
void ModuleNetworkingServer::OnStart()
{
	if (!CreateSocket())
		return;

	// Reuse address
	int enable = 1;
	int res = setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));	
	if (res == SOCKET_ERROR)
	{
		ReportError("ModuleNetworkingServer::start() - setsockopt");
		Disconnect();
		return;
	}

	// Create and bind to local address
	if (!BindSocketToPort(m_ListenPort))
		return;

	m_State = ServerState::LISTENING;
	m_LastPingSent = 0.0f;
}

void ModuleNetworkingServer::OnGUI()
{
	if (ImGui::CollapsingHeader("ModuleNetworkingServer", ImGuiTreeNodeFlags_DefaultOpen))
	{		
		ImGui::NewLine();
		ImGui::Text("Connection checking info:");
		ImGui::Text(" - Ping interval (s): %f", PING_INTERVAL_SECONDS);
		ImGui::Text(" - Disconnection timeout (s): %f", DISCONNECT_TIMEOUT_SECONDS);
		ImGui::Separator();
		ImGui::NewLine();

		if (m_State == ServerState::LISTENING)
		{
			int count = 0;
			for (int i = 0; i < MAX_CLIENTS; ++i)
			{
				if (m_ClientProxies[i].connected)
				{
					ImGui::Text("CLIENT %d", count++);
					ImGui::Text(" - address: %d.%d.%d.%d",
						m_ClientProxies[i].address.sin_addr.S_un.S_un_b.s_b1,
						m_ClientProxies[i].address.sin_addr.S_un.S_un_b.s_b2,
						m_ClientProxies[i].address.sin_addr.S_un.S_un_b.s_b3,
						m_ClientProxies[i].address.sin_addr.S_un.S_un_b.s_b4);

					ImGui::Text(" - port: %d", ntohs(m_ClientProxies[i].address.sin_port));
					ImGui::Text(" - name: %s", m_ClientProxies[i].name.c_str());
					ImGui::Text(" - id: %d", m_ClientProxies[i].clientId);
					
					if (m_ClientProxies[i].gameObject != nullptr)
						ImGui::Text(" - gameObject net id: %d", m_ClientProxies[i].gameObject->networkId);
					else
						ImGui::Text(" - gameObject net id: (null)");
					
					ImGui::Separator();
				}
			}

			ImGui::Checkbox("Render colliders", &App->modRender->mustRenderColliders);
		}
	}
}

void ModuleNetworkingServer::OnPacketReceived(const InputMemoryStream &packet, const sockaddr_in &fromAddress)
{
	if (m_State == ServerState::LISTENING)
	{
		uint32 protoId;
		packet >> protoId;
		if (protoId != PROTOCOL_ID) return;

		ClientProxy* proxy = GetClientProxy(fromAddress);
		ClientMessage message;
		packet >> message;

		switch (message)
		{
			case ClientMessage::HELLO: 
			{
					if (proxy == nullptr)
					{
						proxy = CreateClientProxy();
						if (proxy != nullptr) {
							std::string playerName;
							uint8 spaceshipType;
							packet >> playerName;
							packet >> spaceshipType;

							proxy->address.sin_family = fromAddress.sin_family;
							proxy->address.sin_addr.S_un.S_addr = fromAddress.sin_addr.S_un.S_addr;
							proxy->address.sin_port = fromAddress.sin_port;
							proxy->connected = true;
							proxy->name = playerName;
							proxy->clientId = m_NextClientId++;
							proxy->lastPing = Time.time;
							proxy->serverDelegate = new ServerDelegate(&proxy->repServer);

							// Create new network object
							vec2 initialPosition = 500.0f * vec2{ Random.next() - 0.5f, Random.next() - 0.5f };
							float initialAngle = 360.0f * Random.next();
							proxy->gameObject = SpawnPlayer(spaceshipType, initialPosition, initialAngle);
							proxy->spaceship_type = spaceshipType;
						}
							// NOTE(jesus): Else, if Server is full...
					}

					if (proxy != nullptr)
					{
						// Send welcome to the new player
						OutputMemoryStream welcomePacket;
						welcomePacket << PROTOCOL_ID;
						welcomePacket << ServerMessage::WELCOME;
						welcomePacket << proxy->clientId;
						welcomePacket << proxy->gameObject->networkId;
						SendPacket(welcomePacket, fromAddress);

						// Send all network objects to the new player
						uint16 networkGameObjectsCount;
						GameObject* networkGameObjects[MAX_NETWORK_OBJECTS];
						App->modLinkingContext->GetNetworkGameObjects(networkGameObjects, &networkGameObjectsCount);

						for (uint16 i = 0; i < networkGameObjectsCount; ++i) {
							GameObject* gameObject = networkGameObjects[i];
							// TODO(you): World state replication lab session
							if (gameObject != proxy->gameObject)
								proxy->repServer.Create(gameObject->networkId);
						}

						CONSOLE_INFO_LOG("Message received: hello - from player %s", proxy->name.c_str());
					}
					else
					{
						OutputMemoryStream unwelcomePacket;
						unwelcomePacket << PROTOCOL_ID;
						unwelcomePacket << ServerMessage::UNWELCOME;
						SendPacket(unwelcomePacket, fromAddress);

						CONSOLE_WARN_LOG("Message received: UNWELCOMED hello - server is full");
					}
					break;
			}
			case ClientMessage::INPUT: 
			{
					// Process the input packet and update the corresponding game object
					if (proxy != nullptr && IsValid(proxy->gameObject))
					{
						// TODO(you): Reliability on top of UDP lab session
						// TODO(you): Reliability on top of UDP lab session
						// Read input data						
						while (packet.RemainingByteCount() > 0)
						{
							InputPacketData inputData;
							packet >> inputData.sequenceNumber;
							packet >> inputData.horizontalAxis;
							packet >> inputData.verticalAxis;
							packet >> inputData.buttonBits;

							if (inputData.sequenceNumber >= proxy->nextExpectedInputSequenceNumber)
							{
								proxy->gamepad.horizontalAxis = inputData.horizontalAxis;
								proxy->gamepad.verticalAxis = inputData.verticalAxis;
								UnpackInputControllerButtons(inputData.buttonBits, proxy->gamepad);

								proxy->gameObject->behaviour->OnInput(proxy->gamepad);
								proxy->nextExpectedInputSequenceNumber = inputData.sequenceNumber + 1;

								proxy->m_LastSequenceNumProcessed = inputData.sequenceNumber;
								//CONSOLE_WARN_LOG("Input Processed: %i", inputData.sequenceNumber);
							}
						}
					}
					break;
			}

			// TODO(you): UDP virtual connection lab session
			case ClientMessage::PING:
			{
				if (proxy == nullptr)
					break;

				bool processAcks = false;
				packet >> processAcks;
				if (processAcks)
					proxy->delManager.ProcessAckdSequenceNumbers(packet);				

				proxy->lastPing = Time.time;
				break;
			}
		
		} //switch (message)
	}
}

void ModuleNetworkingServer::OnUpdate()
{
	if (m_State == ServerState::LISTENING)
	{
		// Handle networked game object destructions
		for (DelayedDestroyEntry &destroyEntry : m_NetGameObjectsToDestroyWithDelay)
		{
			if (destroyEntry.object != nullptr)
			{
				destroyEntry.delaySeconds -= Time.deltaTime;
				if (destroyEntry.delaySeconds <= 0.0f)
				{
					DestroyNetworkObject(destroyEntry.object);
					destroyEntry.object = nullptr;
				}
			}
		}

		bool should_ping = Time.time - m_LastPingSent >= PING_INTERVAL_SECONDS;
		bool should_rep = Time.time - m_LastRepSent >= REPLICATION_INTERVAL_SECONDS;

		OutputMemoryStream ping_packet;
		ping_packet << PROTOCOL_ID;
		ping_packet << ServerMessage::PING;

		if (should_ping)
			m_LastPingSent = Time.time;

		if (should_rep)
			m_LastRepSent = Time.time;

		// Update scoreboard
		player_scoreboard.Clear();
		for (ClientProxy& clientProxy : m_ClientProxies) {
			if (clientProxy.connected)
				player_scoreboard.TryAdd(clientProxy.score, clientProxy.name);
		}

		for (ClientProxy &clientProxy : m_ClientProxies)
		{
			if (clientProxy.connected)
			{
				if (Time.time - clientProxy.lastPing >= DISCONNECT_TIMEOUT_SECONDS) {
					CONSOLE_WARN_LOG("Warning: Player \"%s\" disconnected, connection timeout", clientProxy.name.c_str());
					clientProxy.connected = false;
					DestroyClientProxy(&clientProxy);
					continue;
				}

				// TODO(you): UDP virtual connection lab session
				if (should_ping)
					SendPacket(ping_packet, clientProxy.address);

				// Don't let the client proxy point to a destroyed game object
				if (!IsValid(clientProxy.gameObject))
					clientProxy.gameObject = nullptr;

				// TODO(you): World state replication lab session
				// TODO(you): Reliability on top of UDP lab session
				// Here call the Replication::Write()
				// Then send to client
				if (should_rep)
				{
					OutputMemoryStream rep_packet;
					rep_packet << PROTOCOL_ID << ServerMessage::REPLICATION;
					clientProxy.delManager.WriteSequenceNumber(rep_packet, clientProxy.serverDelegate);
					rep_packet << clientProxy.m_LastSequenceNumProcessed;
					
					rep_packet << clientProxy.score;
					rep_packet << player_scoreboard;
					clientProxy.repServer.Write(rep_packet);
					SendPacket(rep_packet, clientProxy.address);
				}

				clientProxy.delManager.ProcessTimedOutPackets();
			}
		}
	}
}

void ModuleNetworkingServer::OnConnectionReset(const sockaddr_in & fromAddress)
{
	// Find the client proxy
	ClientProxy *proxy = GetClientProxy(fromAddress);

	// Clear the client proxy
	if (proxy)
		DestroyClientProxy(proxy);
}

void ModuleNetworkingServer::OnDisconnect()
{
	uint16 netGameObjectsCount;
	GameObject* netGameObjects[MAX_NETWORK_OBJECTS];

	App->modLinkingContext->GetNetworkGameObjects(netGameObjects, &netGameObjectsCount);
	for (uint32 i = 0; i < netGameObjectsCount; ++i)
		NetworkDestroy(netGameObjects[i]);

	for (ClientProxy &clientProxy : m_ClientProxies)
		DestroyClientProxy(&clientProxy);
	
	m_NextClientId = 0;
	m_State = ServerState::STOPPED;
}



// --- Client proxies ---
ModuleNetworkingServer::ClientProxy* ModuleNetworkingServer::CreateClientProxy()
{
	// If it does not exist, pick an empty entry
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (!m_ClientProxies[i].connected)
			return &m_ClientProxies[i];
	}

	return nullptr;
}

ModuleNetworkingServer::ClientProxy * ModuleNetworkingServer::GetClientProxy(const sockaddr_in &clientAddress)
{
	// Try to find the client
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (m_ClientProxies[i].address.sin_addr.S_un.S_addr == clientAddress.sin_addr.S_un.S_addr && m_ClientProxies[i].address.sin_port == clientAddress.sin_port)
			return &m_ClientProxies[i];
	}

	return nullptr;
}

void ModuleNetworkingServer::DestroyClientProxy(ClientProxy* clientProxy)
{
	// Destroy the server delegate.
	delete clientProxy->serverDelegate;

	// Destroy the object from all clients
	if (IsValid(clientProxy->gameObject))
		DestroyNetworkObject(clientProxy->gameObject);

    *clientProxy = {};
}



// --- Spawning ---
GameObject * ModuleNetworkingServer::SpawnPlayer(uint8 spaceshipType, vec2 initialPosition, float initialAngle)
{
	// Create a new game object with the player properties
	GameObject *gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 100, 100 };
	gameObject->angle = initialAngle;

	// Create sprite
	gameObject->sprite = App->modRender->AddSprite(gameObject);
	gameObject->sprite->order = 5;

	if (spaceshipType == 0)
		gameObject->sprite->texture = App->modResources->spacecraft1;
	else if (spaceshipType == 1)
		gameObject->sprite->texture = App->modResources->spacecraft2;
	else
		gameObject->sprite->texture = App->modResources->spacecraft3;

	// Create collider
	gameObject->collider = App->modCollision->AddCollider(ColliderType::PLAYER, gameObject);
	gameObject->collider->isTrigger = true; // NOTE(jesus): This object will receive onCollisionTriggered events

	// Create behaviour
	Spaceship * spaceshipBehaviour = App->modBehaviour->AddSpaceship(gameObject);
	gameObject->behaviour = spaceshipBehaviour;
	gameObject->behaviour->isServer = true;
	return gameObject;
}

void ModuleNetworkingServer::RespawnPlayer(GameObject* prev_GO, vec2 initialPosition, float initialAngle)
{
	// Create a new game object with the player properties
	GameObject* gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 100, 100 };
	gameObject->angle = initialAngle;

	// Create sprite
	gameObject->sprite = App->modRender->AddSprite(gameObject);
	gameObject->sprite->order = 5;

	// Create collider
	gameObject->collider = App->modCollision->AddCollider(ColliderType::PLAYER, gameObject);
	gameObject->collider->isTrigger = true; // NOTE(jesus): This object will receive onCollisionTriggered events

	// Create behaviour
	Spaceship* spaceshipBehaviour = App->modBehaviour->AddSpaceship(gameObject);
	gameObject->behaviour = spaceshipBehaviour;
	gameObject->behaviour->isServer = true;

	for (ClientProxy& p : m_ClientProxies)
	{
		if (p.connected && p.gameObject == prev_GO)
		{
			if (p.spaceship_type == 0)
				gameObject->sprite->texture = App->modResources->spacecraft1;
			else if (p.spaceship_type == 1)
				gameObject->sprite->texture = App->modResources->spacecraft2;
			else
				gameObject->sprite->texture = App->modResources->spacecraft3;

			p.gameObject = gameObject;
			OutputMemoryStream packet;
			packet << PROTOCOL_ID << ServerMessage::RESPAWN << p.gameObject->networkId;
			SendPacket(packet, p.address);
		}
	}
}

void ModuleNetworkingServer::AddScorePlayer(uint32 tag) {
	for (ClientProxy& proxy : m_ClientProxies){
		if (!proxy.connected)
			continue;
		if (proxy.gameObject && proxy.gameObject->tag == tag) {
			proxy.score += POINTS_FOR_KILL;
			break;
		}
	}
}



// --- Update / destruction ---
GameObject * ModuleNetworkingServer::InstantiateNetworkObject()
{
	// Create an object into the server
	GameObject * gameObject = Instantiate();

	// Register the object into the linking context
	App->modLinkingContext->RegisterNetworkGameObject(gameObject);

	// Notify all client proxies' replication manager to create the object remotely
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (m_ClientProxies[i].connected)
		{
			// TODO(you): World state replication lab session
			m_ClientProxies[i].repServer.Create(gameObject->networkId);
		}
	}

	return gameObject;
}

void ModuleNetworkingServer::UpdateNetworkObject(GameObject * gameObject)
{
	// Notify all client proxies' replication manager to update the object remotely
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (m_ClientProxies[i].connected)
		{
			// TODO(you): World state replication lab session
			m_ClientProxies[i].repServer.Update(gameObject->networkId);
		}
	}
}

void ModuleNetworkingServer::DestroyNetworkObject(GameObject * gameObject)
{
	// Notify all client proxies' replication manager to destroy the object remotely
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (m_ClientProxies[i].connected)
		{
			// TODO(you): World state replication lab session
			m_ClientProxies[i].repServer.Destroy(gameObject->networkId);
		}
	}

	// Assuming the message was received, unregister the network identity
	App->modLinkingContext->UnregisterNetworkGameObject(gameObject);

	// Finally, destroy the object from the server
	Destroy(gameObject);
}

void ModuleNetworkingServer::DestroyNetworkObject(GameObject * gameObject, float delaySeconds)
{
	uint32 emptyIndex = MAX_GAME_OBJECTS;
	for (uint32 i = 0; i < MAX_GAME_OBJECTS; ++i)
	{
		if (m_NetGameObjectsToDestroyWithDelay[i].object == gameObject)
		{
			float currentDelaySeconds = m_NetGameObjectsToDestroyWithDelay[i].delaySeconds;
			m_NetGameObjectsToDestroyWithDelay[i].delaySeconds = min(currentDelaySeconds, delaySeconds);
			return;
		}
		else if (m_NetGameObjectsToDestroyWithDelay[i].object == nullptr)
		{
			if (emptyIndex == MAX_GAME_OBJECTS)
				emptyIndex = i;
		}
	}

	ASSERT(emptyIndex < MAX_GAME_OBJECTS);
	m_NetGameObjectsToDestroyWithDelay[emptyIndex].object = gameObject;
	m_NetGameObjectsToDestroyWithDelay[emptyIndex].delaySeconds = delaySeconds;
}



// --- Global create / update / destruction of network game objects ---
GameObject * NetworkInstantiate()
{
	ASSERT(App->modNetServer->IsConnected());
	return App->modNetServer->InstantiateNetworkObject();
}

void NetworkUpdate(GameObject * gameObject)
{
	ASSERT(App->modNetServer->IsConnected());
	ASSERT(gameObject->networkId != 0);

	App->modNetServer->UpdateNetworkObject(gameObject);
}

void NetworkDestroy(GameObject * gameObject)
{
	NetworkDestroy(gameObject, 0.0f);
}

void NetworkDestroy(GameObject * gameObject, float delaySeconds)
{
	ASSERT(App->modNetServer->IsConnected());
	ASSERT(gameObject->networkId != 0);

	App->modNetServer->DestroyNetworkObject(gameObject, delaySeconds);
}