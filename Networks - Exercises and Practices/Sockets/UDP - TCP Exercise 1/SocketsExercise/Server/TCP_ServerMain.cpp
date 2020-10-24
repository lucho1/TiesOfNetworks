// -- Socket includes --
#include <winsock2.h>
#include <WS2tcpip.h>
//#include <iphlpapi.h>

// -- System Includes --
#include <Windows.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")


// -- Global Variables --
u_short ConnectionPort = 6060;

int main(int argc, char** argv)
{
	std::cout << "--- TCP SERVER STARTUP ---" << std::endl << std::endl;
	
	// --- Initialize WinSock ---
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	// --- Create LISTENING Socket (TCP) ---
	sockaddr_in listenerSocket_address;
	listenerSocket_address.sin_family = AF_INET;
	listenerSocket_address.sin_port = htons(ConnectionPort);
	listenerSocket_address.sin_addr.S_un.S_addr = INADDR_ANY;

	SOCKET listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// --- Bind Socket & Make it Listen ---
	bind(listener_socket, (sockaddr*)&listenerSocket_address, sizeof(listenerSocket_address));
	listen(listener_socket, SOMAXCONN);

	// --- Wait for Connections ---
	listener_socket = accept(listener_socket, NULL, NULL);

	// --- Receive Message ---
	std::string msg = "Server Says: Pong";
	char buffer[1024]{ 0 };

	for (int i = 0; i < 5; ++i)
	{
		recv(listener_socket, buffer, sizeof(buffer), 0);
		send(listener_socket, msg.c_str(), msg.size() + 1, 0);
		std::cout << "[SERVER]: Received Message (Iteration " << i << "):" << std::endl;
		std::cout << "\t" << buffer << std::endl << std::endl;
	}

	// --- Pause (before clean!) ---
	system("pause");
	// --- Clean Up ---
	closesocket(listener_socket);
	WSACleanup();
	return 0;
}