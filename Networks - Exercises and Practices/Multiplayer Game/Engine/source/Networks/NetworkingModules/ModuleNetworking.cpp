#include "Core/Core.h"
#include "ModuleNetworking.h"


void NetworkDisconnect()
{
	if (App->modNetClient->IsConnected())
		App->modNetClient->Disconnect();

	if (App->modNetServer->IsConnected())
		App->modNetServer->Disconnect();
}

// --- ModuleNetworking protected methods ---
void ModuleNetworking::ReportError(const char* inOperationDesc)
{
	LPVOID lpMsgBuf;
	DWORD errorNum = WSAGetLastError();

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	CONSOLE_ERROR_LOG("Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
}

bool ModuleNetworking::CreateSocket()
{
	// Create
	m_Socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_Socket == INVALID_SOCKET)
	{
		ReportError("ModuleNetworking::createSocket() - socket");
		return false;
	}

	// Set non-blocking mode
	bool enableBlockingMode = true;
	u_long arg = enableBlockingMode ? 1 : 0;
	int res = ioctlsocket(m_Socket, FIONBIO, &arg);

	if (res == SOCKET_ERROR)
	{
		ReportError("ModuleNetworking::createSocket() - ioctlsocket FIONBIO");
		Disconnect();
		return false;
	}

	return true;
}

bool ModuleNetworking::BindSocketToPort(int port)
{
	sockaddr_in local_address;
	local_address.sin_addr.S_un.S_addr = INADDR_ANY;
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(port);

	int res = bind(m_Socket, (sockaddr*)&local_address, sizeof(local_address));
	if (res == SOCKET_ERROR)
	{
		ReportError("ModuleNetworkingServer::start() - bind");
		Disconnect();
		return false;
	}

	return true;
}

void ModuleNetworking::SendPacket(const OutputMemoryStream & packet, const sockaddr_in &destAddress)
{
	SendPacket(packet.GetBufferPtr(), packet.GetSize(), destAddress);
}

void ModuleNetworking::SendPacket(const char * data, uint32 size, const sockaddr_in &destAddress)
{
	ASSERT(size <= DEFAULT_PACKET_SIZE, "Increase DEFAULT_PACKET_SIZE if not enough");

	int byteSentCount = sendto(m_Socket, (const char*)data, size, 0, (sockaddr*)&destAddress, sizeof(destAddress));
	if (byteSentCount <= 0)
		ReportError("ModuleNetworking::sendPacket() - sendto");
	else
		m_SentPacketsCount++;
}


// --- Module virtual methods ---
bool ModuleNetworking::Init()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	if (WSAStartup(version, &data) == SOCKET_ERROR)
	{
		ReportError("ModuleNetworking::init() - WSAStartup");
		return false;
	}

	player_scoreboard.SetCapacity(5);

	SimulatedRealWorldConditions_Init();
	return true;
}

bool ModuleNetworking::Start()
{
	m_SentPacketsCount = 0;
	m_ReceivedPacketsCount = 0;	
	OnStart();
	return true;
}

bool ModuleNetworking::PreUpdate()
{
	if (m_Socket == INVALID_SOCKET)
		return true;

	BEGIN_TIMED_BLOCK(NetRecv);	
	ProcessIncomingPackets();
	END_TIMED_BLOCK(NetRecv);
	return true;
}

bool ModuleNetworking::Update()
{
	if (m_Socket == INVALID_SOCKET)
		return true;

	BEGIN_TIMED_BLOCK(NetSend);
	OnUpdate();
	END_TIMED_BLOCK(NetSend);
	return true;
}

bool ModuleNetworking::DrawGUI()
{
	if (IsConnected())
	{
		ImGui::Begin("ModuleNetworking window");
		
		ImGui::Text(" - Current time: %f", Time.time);
		ImGui::Text(" - # Packet sent: %u", m_SentPacketsCount);
		ImGui::Text(" - # Packet received: %u", m_ReceivedPacketsCount);
		ImGui::Text(" - # Networked objects: %u", App->modLinkingContext->GetNetworkGameObjectsCount());

		if (ImGui::Button("Disconnect"))
			Disconnect();

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

		// Simulate real world conditions
		if (ImGui::CollapsingHeader("Simulate real world conditions", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Simulate latency / jitter", &m_SimulateLatency);
			if (m_SimulateLatency)
			{
				ImGui::InputFloat("Max. latency (s)", &m_SimulatedLatency, 0.001f, 0.01f, 4);
				ImGui::InputFloat("Max. jitter (s)", &m_SimulatedJitter, 0.001f, 0.01f, 4);
				
				if (ImGui::Button("Reset##defaults_latency_jitter"))
				{
					m_SimulatedLatency = 0.07f;
					m_SimulatedJitter = 0.03f;
				}
			}
			
			ImGui::Checkbox("Simulate packet drops", &m_SimulateDrops);
			if (m_SimulateDrops)
			{
				ImGui::InputFloat("Drop ratio", &m_SimulatedDropRatio, 0.01f, 0.1f, 4);
				if (ImGui::Button("Reset##default_drop_ratio"))
					m_SimulatedDropRatio = 0.01f;
				
				ImGui::Text(" # Dropped packets: %d", m_SimulatedPacketsDropped);
				ImGui::Text(" # Received packets: %d", m_SimulatedPacketsReceived);
			}

			ImGui::Text("Simulated buffer usage: %d%%", 100 * m_PendingSimulatedPacketsCount / MAX_SIMULATED_PACKETS);
		}

		OnGUI();
		ImGui::PopItemWidth();
		ImGui::End();
	}

	return true;
}

bool ModuleNetworking::Stop()
{
	OnDisconnect();
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;

	m_SentPacketsCount = 0;
	m_ReceivedPacketsCount = 0;
	m_SimulatedRandom = RandomNumberGenerator();

	return true;
}

bool ModuleNetworking::CleanUp()
{
	if (WSACleanup() == SOCKET_ERROR)
	{
		ReportError("ModuleNetworking::cleanUp() - WSACleanup");
		return false;
	}

	return true;
}


// --- Incoming/Outgoing packet processing ---
void ModuleNetworking::ProcessIncomingPackets()
{
	// Handle incoming packets
	while (true)
	{
		sockaddr_in fromAddress = {};
		socklen_t fromLength = sizeof(fromAddress);
		InputMemoryStream inPacket;

		int readByteCount = recvfrom(m_Socket, (char*)inPacket.GetBufferPtr(), inPacket.GetCapacity(), 0, (sockaddr*)&fromAddress, &fromLength);
		if (readByteCount > 0)
		{
			inPacket.SetSize(readByteCount);

			if (m_SimulateLatency || m_SimulateDrops)
				SimulatedRealWorldConditions_EnqueuePacket(inPacket, fromAddress);
			else
			{
				m_ReceivedPacketsCount++;
				OnPacketReceived(inPacket, fromAddress);
			}
		}
		else
		{
			int error = WSAGetLastError();
			if (readByteCount == 0)
				OnConnectionReset(fromAddress); // Graceful disconnection from remote socket
			else if (error == WSAECONNRESET)
			{
				//this can happen if a remote socket closed and we haven't DC'd yet.
				//this is the ICMP message being sent back saying the port on that computer is closed
				char fromAddressStr[64];
				inet_ntop(AF_INET, &fromAddress.sin_addr, fromAddressStr, sizeof(fromAddressStr));

				CONSOLE_WARN_LOG("ModuleNetworking::processIncomingPackets() - Connection reset from %s:%d", fromAddressStr, ntohs(fromAddress.sin_port));
				OnConnectionReset(fromAddress);
			}
			else if (error != WSAEWOULDBLOCK)
				ReportError("ModuleNetworking::processIncomingPackets() - recvfrom");

			break;

			// NOTE(jesus): WSAEWOULDBLOCK is not an error for us, as the socket is configured in
			// non-blocking mode. This means that there was no incoming data available
			// when recvfrom was executed.
		}
	}

	if (m_SimulateLatency || m_SimulateDrops)
		SimulatedRealWorldConditions_ProcessQueuedPackets();
}


// --- Real world conditions simulation ---
void ModuleNetworking::SimulatedRealWorldConditions_Init()
{
	m_FreeSimulatedPackets = nullptr;
	m_PendingSimulatedPackets = nullptr;
	m_PendingSimulatedPacketsCount = 0;
	for (uint32 i = ArrayCount(m_SimulatedPackets); i > 0; --i)
	{
		SimulatedPacket *next = m_FreeSimulatedPackets;
		m_FreeSimulatedPackets = &m_SimulatedPackets[i-1];
		m_FreeSimulatedPackets->next = next;
	}
}

void ModuleNetworking::SimulatedRealWorldConditions_EnqueuePacket(const InputMemoryStream &packet, const sockaddr_in &fromAddress)
{
	float packetChance = m_SimulatedRandom.next();
	if (!m_SimulateDrops)
		packetChance = 1.0f;

	if (packetChance > m_SimulatedDropRatio)
	{
		float randomJitterFactor = 2.0f * m_SimulatedRandom.next() - 1.0f; // from -1 to 1
		double receptionTime = Time.time + m_SimulatedLatency + m_SimulatedJitter * randomJitterFactor;

		if (!m_SimulateLatency)
			receptionTime = Time.time;

		SimulatedPacket *simulatedPacket = m_FreeSimulatedPackets;
		ASSERT(simulatedPacket != nullptr);

		m_FreeSimulatedPackets = simulatedPacket->next;
		std::memcpy((void*)simulatedPacket->packet.GetBufferPtr(), packet.GetBufferPtr(), packet.GetSize());
		
		simulatedPacket->packet.Clear();
		simulatedPacket->packet.SetSize(packet.GetSize());
		simulatedPacket->fromAddress = fromAddress;
		simulatedPacket->receptionTime = receptionTime;
		
		if (m_PendingSimulatedPackets == nullptr || m_PendingSimulatedPackets->receptionTime > receptionTime)
		{
			simulatedPacket->next = m_PendingSimulatedPackets;
			m_PendingSimulatedPackets = simulatedPacket;
		}
		else
		{
			SimulatedPacket* currentSimulatedPacket = m_PendingSimulatedPackets;
			for (; currentSimulatedPacket != nullptr; currentSimulatedPacket = currentSimulatedPacket->next)
			{
				if (currentSimulatedPacket->next == nullptr || currentSimulatedPacket->next->receptionTime > receptionTime)
				{
					simulatedPacket->next = currentSimulatedPacket->next;
					currentSimulatedPacket->next = simulatedPacket;
					break;
				}
			}
		}

		m_PendingSimulatedPacketsCount++;
		m_SimulatedPacketsReceived++;
	}
	else
		m_SimulatedPacketsDropped++;
}

void ModuleNetworking::SimulatedRealWorldConditions_ProcessQueuedPackets()
{
	while (m_PendingSimulatedPackets != nullptr)
	{
		SimulatedPacket *simulatedPacket = m_PendingSimulatedPackets;
		if (simulatedPacket->receptionTime <= Time.time)
		{
			m_ReceivedPacketsCount++;
			OnPacketReceived(simulatedPacket->packet, simulatedPacket->fromAddress);

			m_PendingSimulatedPackets = simulatedPacket->next;
			simulatedPacket->next = m_FreeSimulatedPackets;
			m_FreeSimulatedPackets = simulatedPacket;

			ASSERT(m_PendingSimulatedPacketsCount > 0);
			m_PendingSimulatedPacketsCount--;
		}
		else
			break;
	}
}