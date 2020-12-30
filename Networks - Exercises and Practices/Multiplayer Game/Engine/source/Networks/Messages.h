#ifndef _MESSAGES_H_
#define _MESSAGES_H_

enum class ClientMessage : uint8
{
	HELLO,
	INPUT,
	PING		// NOTE(jesus): Use this message type in the virtual connection lab session
};

enum class ServerMessage : uint8
{
	WELCOME,
	UNWELCOME,
	PING,
	REPLICATION // NOTE(jesus): Use this message type in the virtual connection lab session
};

#endif //_MESSAGES_H_