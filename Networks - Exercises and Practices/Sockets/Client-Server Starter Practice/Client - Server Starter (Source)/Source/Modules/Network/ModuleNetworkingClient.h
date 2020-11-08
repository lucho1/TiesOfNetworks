#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingClient : public ModuleNetworking
{
public:
	ModuleNetworkingClient();

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
	bool DrawUI_SendButton();
	void ParseMessage(const std::string& buffer);

private:

	// Client state
	enum class ClientState
	{
		STOPPED,
		START,
		LOGGING,
		REJECTED,
		ONLINE
	};

	// Variables
	std::string m_ClientName;
	uint m_ClientID;
	ClientState m_ClientState = ClientState::STOPPED;
	SOCKET m_Socket = INVALID_SOCKET;

	std::string m_ServerAddressStr = {};
	std::string m_ServerName = "ServerName";
	Color m_UserTextColor = Colors::ConsoleGreen;
	std::unordered_map<std::string, uint> m_ConnectedUsers;
	std::unordered_map<std::string, CLIENT_COMMANDS> m_UserCommands;
	std::unordered_map<CLIENT_COMMANDS, std::string> m_UserCmdDescriptions;

	std::string reject_message;

	bool m_ServerDisconnection = false;
};