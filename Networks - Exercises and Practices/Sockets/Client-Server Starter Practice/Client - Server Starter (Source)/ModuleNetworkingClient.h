#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingClient : public ModuleNetworking
{
public:

	// ModuleNetworkingClient public methods
	bool start(const char *serverAddress, int serverPort, const char *clientName);
	bool isRunning() const;


private:

	// Virtual functions of Modules
	bool update() override;
	bool gui() override;

	// Virtual functions of ModuleNetworking
	void onSocketReceivedData(SOCKET socket, byte * data) override;
	void onSocketDisconnected(SOCKET socket) override;


private:

	// Client state
	enum class ClientState
	{
		Stopped,
		Start,
		Logging
	};

	// Variables
	std::string m_ClientName;
	ClientState m_ClientState = ClientState::Stopped;
	SOCKET m_Socket = INVALID_SOCKET;

	sockaddr_in m_ServerAddress = {};
};

