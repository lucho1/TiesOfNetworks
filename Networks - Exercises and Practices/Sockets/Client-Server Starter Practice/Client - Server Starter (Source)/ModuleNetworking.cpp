#include "Networks.h"
#include "ModuleNetworking.h"


static uint8 NumModulesUsingWinsock = 0;

void ModuleNetworking::ReportErrorAndClose(const SOCKET s, const std::string& message, const std::string& socket_or_side_name, const char* function_name)
{
	reportError((message + " on " + function_name + "... CLOSING SOCKET").c_str());
	if (closesocket(s) == SOCKET_ERROR)
	{
		std::string str = "[NET]: Error closing '" + socket_or_side_name + "' socket on '" + function_name + "' function";
		ERROR_LOG(str.c_str());
	}
}

void ModuleNetworking::reportError(const char* inOperationDesc)
{
	LPVOID lpMsgBuf;
	DWORD errorNum = WSAGetLastError();

	FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, errorNum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL
				  );

	ERROR_LOG("\t%s\n\t\tError Number and Description: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
}

void ModuleNetworking::disconnect()
{
	for (SOCKET socket : m_SocketsVec)
	{
		shutdown(socket, 2);
		closesocket(socket);
	}

	m_SocketsVec.clear();
}

bool ModuleNetworking::init()
{
	if (NumModulesUsingWinsock == 0)
	{
		NumModulesUsingWinsock++;

		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		if (WSAStartup(version, &data) != 0)
		{
			reportError("Error on ModuleNetworking::init() with WSAStartup()");
			return false;
		}
	}

	return true;
}

bool ModuleNetworking::preUpdate()
{
	if (m_SocketsVec.empty())
		return true;

	// NOTE(jesus): You can use this temporary buffer to store data from recv()
	const uint32 incomingDataBufferSize = Kilobytes(1);
	byte incomingDataBuffer[incomingDataBufferSize];

	// TODO(jesus): select those sockets that have a read operation available
	fd_set readfds;
	FD_ZERO(&readfds);
	for (auto s : m_SocketsVec)
		FD_SET(s, &readfds);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	if (select(0, &readfds, nullptr, nullptr, &timeout) == SOCKET_ERROR)
		reportError("Error on selecting sockets at ModuleNetworking::preUpdate()");

	// TODO(jesus): for those sockets selected, check wheter or not they are
	// a listen socket or a standard socket and perform the corresponding
	// operation (accept() an incoming connection or recv() incoming data,
	// respectively).
	// On accept() success, communicate the new connected socket to the
	// subclass (use the callback onSocketConnected()), and add the new
	// connected socket to the managed list of sockets.
	// On recv() success, communicate the incoming data received to the
	// subclass (use the callback onSocketReceivedData()).
	std::list<SOCKET> disconnected_sockets = {};
	for (auto s : m_SocketsVec)
	{
		if (FD_ISSET(s, &readfds))
		{
			if (isListenSocket(s))
			{
				sockaddr_in add;
				int addSize = sizeof(add);
				SOCKET newSocket = accept(s, (sockaddr*)&add, &addSize);

				if (newSocket != INVALID_SOCKET)
				{
					onSocketConnected(newSocket, add);
					addSocket(newSocket);
				}
				else
					ReportErrorAndClose(newSocket, "[NET]: Accept function failed to create new socket", "SERVER", "ModuleNetworking::preUpdate()");
			}
			else
			{
				// TODO(jesus): handle disconnections. Remember that a socket has been
				// disconnected from its remote end either when recv() returned 0,
				// or when it generated some errors such as ECONNRESET.
				// Communicate detected disconnections to the subclass using the callback
				// onSocketDisconnected().					
				int recv_status = recv(s, (char*)incomingDataBuffer, incomingDataBufferSize, 0);

				// Since len is always > 0 (as stated above), we don't need to check for recv_status == 0 && len == 0
				if(recv_status == ECONNRESET || recv_status <= 0)
				{
					disconnected_sockets.push_back(s);
					onSocketDisconnected(s);

					if(recv_status == SOCKET_ERROR)
						LOG("Disconnected Client triggered SOCKET_ERROR, probably due to forced disconnection"); // SOCKET_ERROR = -1, so checking for recv_status <= 0 is fine
				}
				else if (recv_status > 0)																
					onSocketReceivedData(s, incomingDataBuffer);
			}
		}
	}

	// TODO(jesus): Finally, remove all disconnected sockets from the list
	// of managed sockets.
	for (SOCKET s : disconnected_sockets)
		m_SocketsVec.erase(std::find(m_SocketsVec.begin(), m_SocketsVec.end(), s));	

	disconnected_sockets.clear();
	return true;
}

bool ModuleNetworking::cleanUp()
{
	disconnect();

	NumModulesUsingWinsock--;
	if (NumModulesUsingWinsock == 0)
	{
		if (WSACleanup() != 0)
		{
			reportError("Error on ModuleNetworking::cleanUp() with WSACleanup()");
			return false;
		}
	}

	return true;
}

void ModuleNetworking::addSocket(SOCKET socket)
{
	m_SocketsVec.push_back(socket);
}