#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingServer : public ModuleNetworking
{
public:

	// ModuleNetworkingServer public methods
	bool start(int port);
	bool isRunning() const;
	

private:

	// Virtual functions of Modules
	bool update() override;
	bool gui() override;

	// Virtual functions of ModuleNetworking
	bool isListenSocket(SOCKET socket) const override;
	void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) override;
	void onSocketReceivedData(SOCKET socket, byte * data) override;
	void onSocketDisconnected(SOCKET socket) override;
	

private:

	// Server state & Connected Sockets structure
	enum class ServerState
	{
		Stopped,
		Listening
	};

	struct ConnectedSocket
	{
		sockaddr_in address;
		SOCKET socket;
		std::string client_name;
	};

	// Variables
	ServerState m_ServerState = ServerState::Stopped;
	SOCKET m_ListeningSocket = INVALID_SOCKET;
	std::vector<ConnectedSocket> m_ConnectedSockets;
};