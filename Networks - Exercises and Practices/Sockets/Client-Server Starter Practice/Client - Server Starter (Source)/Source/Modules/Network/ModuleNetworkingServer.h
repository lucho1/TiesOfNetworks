#pragma once

#include "ModuleNetworking.h"
#include <chrono>

class ModuleNetworkingServer : public ModuleNetworking
{
public:

	// ModuleNetworkingServer public methods
	bool Start(int port, const char* serverName);
	bool IsRunning() const { return m_ServerState != ServerState::STOPPED; }

	const std::pair<std::string, int> GetUserFromID(uint ID);
	const std::pair<std::string, int> GetNextUser(int index);
	const std::pair<std::string, int> GetNextUserFromID(uint current_userID);
	uint GetUsersNumber() const { return m_ConnectedSockets.size(); }

	const std::unordered_map<std::string, uint>& GetUserNicknames() const { return m_ConnectedNicknames; }

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
	std::chrono::steady_clock::time_point not_timer = std::chrono::steady_clock::now();


private:

	// Class Methods
	uint GetSocketIndex(const SOCKET& s);
};