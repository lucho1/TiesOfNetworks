#pragma once

class ModuleNetworking : public Module
{
private:

	// Virtual functions of Modules
	bool init() override;
	bool preUpdate() override;
	bool cleanUp() override;
	bool postUpdate() override;


	// Socket event callbacks
	virtual bool isListenSocket(SOCKET socket) const { return false; }
	virtual void onSocketReceivedData(SOCKET s, const InputMemoryStream& packet) = 0;
	virtual void onSocketDisconnected(SOCKET s) = 0;
	virtual void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) {}

protected:

	// Networking Methods
	bool SendPacket(const OutputMemoryStream& packet, SOCKET s);
	void disconnect();
	
	// Other Protected Functions
	void addSocket(SOCKET socket);
	static void reportError(const char *message);
	static void ReportErrorAndClose(const SOCKET s, const std::string& message, const std::string& socket_or_side_name, const char* function_name);
	
protected:

	// Variables
	std::list<SOCKET> m_DisconnectedSockets = {};
	std::vector<SOCKET> m_SocketsVec;
	bool flagServerDisconnect = false;
};