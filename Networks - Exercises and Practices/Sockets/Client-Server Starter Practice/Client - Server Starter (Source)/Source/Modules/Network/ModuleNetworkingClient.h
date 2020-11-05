#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingClient : public ModuleNetworking
{
public:

	// ModuleNetworkingClient public methods
	bool Start(const char *serverAddress, int serverPort, const char *clientName);
	bool IsRunning() const;

private:

	// Virtual functions of Modules
	virtual bool Update() override;
	virtual bool GUI() override;

	// Virtual functions of ModuleNetworking - Callbacks
	virtual void onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet) override;
	virtual void onSocketDisconnected(SOCKET socket) override;

	// Private Module Methods
	void SetupPacket(OutputMemoryStream& packet, CLIENT_MESSAGE msg_type, std::string msg, const Color& msg_color);
	void ReadPacket(const InputMemoryStream& packet, SERVER_MESSAGE& msg_type, std::string& msg, uint& src_id, Color& msg_color);
	bool DrawUI_SendButton();

private:

	// Client state
	enum class ClientState
	{
		STOPPED,
		START,
		LOGGING
	};

	// Variables
	std::string m_ClientName;
	ClientState m_ClientState = ClientState::STOPPED;
	SOCKET m_Socket = INVALID_SOCKET;

	std::string m_ServerAddressStr = {};
	std::string m_ServerName = "ServerName";
	Color m_UserTextColor = Colors::ConsoleGreen;

	bool m_ServerDisconnection = false;
};