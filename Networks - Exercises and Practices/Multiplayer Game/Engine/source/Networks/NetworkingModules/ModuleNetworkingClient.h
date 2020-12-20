#ifndef _MODULE_NETWORKINGCLIENT_H_
#define _MODULE_NETWORKINGCLIENT_H_

#include "ModuleNetworking.h"

class ModuleNetworkingClient : public ModuleNetworking
{
public:

	// ModuleNetworkingClient public methods
	void SetServerAddress(const char *serverAddress, uint16 serverPort);
	void SetPlayerInfo(const char *playerName, uint8 spaceshipType);


private:

	// ModuleNetworking virtual methods
	virtual bool IsClient() const override { return true; }

	virtual void OnStart() override;
	virtual void OnUpdate() override;
	virtual void OnGUI() override;
	virtual void OnDisconnect() override;
	
	virtual void OnPacketReceived(const InputMemoryStream &packet, const sockaddr_in &fromAddress) override;
	virtual void OnConnectionReset(const sockaddr_in& fromAddress) override { Disconnect(); }

private:

	// Client state
	enum class ClientState
	{
		STOPPED,
		CONNECTING,
		CONNECTED
	};

	ClientState m_State = ClientState::STOPPED;

	std::string m_ServerAddressStr;
	uint16 m_ServerPort = 0;

	sockaddr_in m_ServerAddress = {};
	std::string m_PlayerName = "player";
	uint8 m_SpaceshipType = 0;

	uint32 m_PlayerId = 0;
	uint32 m_NetworkId = 0;


	// Connecting stage
	float m_SecondsSinceLastHello = 0.0f;


	// Input
	static const int MAX_INPUT_DATA_SIMULTANEOUS_PACKETS = 64;

	InputPacketData m_InputData[MAX_INPUT_DATA_SIMULTANEOUS_PACKETS];
	uint32 m_InputDataFront = 0;
	uint32 m_InputDataBack = 0;

	uint32 m_LastReceivedInputNum = -1;

	float m_InputDeliveryIntervalSeconds = 0.05f;
	float m_SecondsSinceLastInputDelivery = 0.0f;

	// Virtual connection
	// TODO(you): UDP virtual connection lab session
	float m_LastPingSent = 0.0f;
	float m_LastPingReceived = 0.0f;

	// Replication
	// TODO(you): World state replication lab session
	ReplicationManagerClient m_RepManager;


	// Delivery manager
	// TODO(you): Reliability on top of UDP lab session


	// Latency management
	// TODO(you): Latency management lab session
};

#endif //_MODULE_NETWORKINGCLIENT_H_