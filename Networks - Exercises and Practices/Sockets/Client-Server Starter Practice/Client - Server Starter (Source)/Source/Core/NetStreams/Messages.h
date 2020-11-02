#pragma once

// Add as many messages as you need depending on the
// functionalities that you decide to implement.

enum class ClientMessage
{
	HELLO,
	CLIENT_TEXT,
	CLIENT_PRIVATE_TEXT,
	CLIENT_COMMAND,
	CLIENT_DISCONNECTION
};

enum class ServerMessage
{
	WELCOME,
	CLIENT_TEXT,
	CLIENT_PRIVATE_TEXT,
	SERVER_INFO,
	SERVER_WARN,
	SERVER_ERROR,
	SERVER_DISCONNECTION
};