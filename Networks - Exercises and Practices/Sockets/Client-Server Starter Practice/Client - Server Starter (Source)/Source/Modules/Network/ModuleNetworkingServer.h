#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingServer : public ModuleNetworking
{
public:

	// ModuleNetworkingServer public methods
	bool Start(int port);
	bool IsRunning() const;
	

private:

	// Virtual functions of Modules
	virtual bool Update() override;
	virtual bool GUI() override;

	// Virtual functions of ModuleNetworking
	virtual inline bool IsListenSocket(SOCKET socket) const override { return socket == m_ListeningSocket; }
	
	// Callbacks
	virtual void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) override;
	virtual void onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet) override;
	virtual void onSocketDisconnected(SOCKET socket) override;

private:

	// Server state & Connected Sockets structure
	enum class ServerState
	{
		STOPPED,
		LISTENING
	};

	struct ConnectedSocket
	{
		sockaddr_in address;
		SOCKET socket;
		std::string client_name;

		int id = 0;
	};

	// Variables
	ServerState m_ServerState = ServerState::STOPPED;
	SOCKET m_ListeningSocket = INVALID_SOCKET;
	std::vector<ConnectedSocket> m_ConnectedSockets;


private:

	// Class Methods
	uint FindSocket(const SOCKET& s);
	const OutputMemoryStream& SetupPacket(SERVER_MESSAGE msg_type, const char* msg, uint src_id, const Color& msg_color);
};