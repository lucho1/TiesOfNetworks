#pragma once

class ModuleNetworking : public Module
{
private:

	// Virtual functions of Modules
	virtual bool Init() override;
	virtual bool PreUpdate() override;
	virtual bool CleanUp() override;
	virtual bool PostUpdate() override;


	// Socket Methods
	virtual inline bool IsListenSocket(SOCKET socket) const { return false; }

	// Callbacks
	virtual void onSocketReceivedData(SOCKET s, const InputMemoryStream& packet) = 0;
	virtual void onSocketDisconnected(SOCKET s) = 0;
	virtual void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) {}

protected:

	// Networking Methods
	bool SendPacket(const OutputMemoryStream& packet, SOCKET s);
	void Disconnect();
	
	// Other Protected Functions
	void AddSocket(SOCKET socket);
	static void ReportError(const char *message);
	static void ReportErrorAndClose(const SOCKET s, const std::string& message, const std::string& socket_or_side_name, const char* function_name);
	
protected:

	// Variables
	std::list<SOCKET> m_DisconnectedSockets = {};
	std::vector<SOCKET> m_SocketsVec;
	bool m_ServerDisconnection = false;
};