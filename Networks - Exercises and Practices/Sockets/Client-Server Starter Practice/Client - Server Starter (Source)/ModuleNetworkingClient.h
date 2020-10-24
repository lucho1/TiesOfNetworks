#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingClient : public ModuleNetworking
{
public:

	// ModuleNetworkingClient public methods
	bool start(const char *serverAddress, int serverPort, const char *playerName);
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
	std::string playerName;
	ClientState state = ClientState::Stopped;
	SOCKET socket = INVALID_SOCKET;

	sockaddr_in serverAddress = {};
};

