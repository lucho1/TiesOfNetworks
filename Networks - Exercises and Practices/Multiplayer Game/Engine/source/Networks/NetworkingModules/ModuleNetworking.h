#ifndef _MODULE_NETWORKING_H_
#define _MODULE_NETWORKING_H_

class ModuleNetworking : public Module
{
public:

	virtual bool IsServer() const	{ return false; }
	virtual bool IsClient() const	{ return false; }

	bool IsConnected() const		{ return m_Socket != INVALID_SOCKET; }
	void Disconnect()				{ Enable(false); }
	
	
protected:

	// ModuleNetworking protected members
	bool CreateSocket();
	bool BindSocketToPort(int port);
	void SendPacket(const OutputMemoryStream &packet, const sockaddr_in &destAddress);
	void SendPacket(const char *data, uint32 size, const sockaddr_in &destAddress);
	void ReportError(const char *message);
	

private:

	// Module virtual methods
	virtual bool Init() override;
	virtual bool Start() override;
	virtual bool PreUpdate() override;
	virtual bool Update() override;
	virtual bool DrawGUI() override;
	virtual bool Stop() override;
	virtual bool CleanUp() override;


	// ModuleNetworking methods
	void ProcessIncomingPackets();
	virtual void OnStart() = 0;
	virtual void OnGUI() = 0;
	virtual void OnPacketReceived(const InputMemoryStream &packet, const sockaddr_in &fromAddress) = 0;
	virtual void OnUpdate() = 0;
	virtual void OnConnectionReset(const sockaddr_in &fromAddress) = 0;
	virtual void OnDisconnect() = 0;


	// Real World Conditions Simulation
	void SimulatedRealWorldConditions_Init();
	void SimulatedRealWorldConditions_EnqueuePacket(const InputMemoryStream& packet, const sockaddr_in& fromAddress);
	void SimulatedRealWorldConditions_ProcessQueuedPackets();


protected:

	SOCKET m_Socket = INVALID_SOCKET;

	// Scoreboard
	Scoreboard player_scoreboard;

private:

	uint32 m_SentPacketsCount = 0;
	uint32 m_ReceivedPacketsCount = 0;

	// Real world conditions simulation
	bool m_SimulateLatency = false;
	bool m_SimulateDrops = false;

	float m_SimulatedLatency = 0.07f;
	float m_SimulatedJitter = 0.03f;
	float m_SimulatedDropRatio = 0.01f;

	uint32 m_SimulatedPacketsReceived = 0;
	uint32 m_SimulatedPacketsDropped = 0;
	

	static const int MAX_SIMULATED_PACKETS = 128;
	struct SimulatedPacket
	{
		InputMemoryStream packet;
		sockaddr_in fromAddress;
		double receptionTime;
		SimulatedPacket* next = nullptr;
	};

	SimulatedPacket m_SimulatedPackets[MAX_SIMULATED_PACKETS];
	SimulatedPacket* m_FreeSimulatedPackets = nullptr;
	SimulatedPacket* m_PendingSimulatedPackets = nullptr;

	RandomNumberGenerator m_SimulatedRandom;
	uint32 m_PendingSimulatedPacketsCount = 0;
};

void NetworkDisconnect();

#endif //_MODULE_NETWORKING_H_