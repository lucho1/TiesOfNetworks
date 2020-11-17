#pragma once

class ScreenGame : public Screen
{

public:

	// Setters
	inline void SetAsServer(bool server)				{ isServer = server; }
	inline void SetServerPort(int port)					{ m_ServerPort = port; }
	inline void SetServerAddress(const char* address)	{ m_ServerAddress = address; }
	inline void SetClientName(const char* name)			{ m_ClientName = name; }
	inline void SetServerName(const char* name)			{ m_ServerName = name; }


private:

	// Virtual functions of Screen
	virtual void Enable() override;
	virtual void Update() override;


private:

	// Variables
	bool isServer = true;
	const char* m_ServerName = "ServerName";
	
	int m_ServerPort = 8888;
	const char* m_ServerAddress = "127.0.0.1";
	const char* m_ClientName = "Username";
};