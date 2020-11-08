#pragma once

#include "ModuleNetworking.h"

class ModuleNetworkingServer : public ModuleNetworking
{
public:

	// ModuleNetworkingServer public methods
	bool Start(int port, const char* serverName);
	bool IsRunning() const { return m_ServerState != ServerState::STOPPED; }

	const std::pair<std::string, uint> GetUserFromID(uint ID);
	const std::pair<std::string, uint> GetNextUser(uint current_userID);
	uint GetUsersNumber() const { return m_ConnectedNicknames.size(); }

	void SendServerNotification(const std::string& msg, EntryType type, int user_id = -1);


private:

	// Virtual functions of Modules
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
	};

	// Variables
	ServerState m_ServerState = ServerState::STOPPED;
	std::string m_ServerName = "ServerName";
	std::string m_ServerAddress = "NULL";
	SOCKET m_ListeningSocket = INVALID_SOCKET;

	std::unordered_map<uint, ConnectedSocket> m_ConnectedSockets;
	std::unordered_map<std::string, uint> m_ConnectedNicknames;


private:

	// Class Methods
	uint GetSocketIndex(const SOCKET& s);
};