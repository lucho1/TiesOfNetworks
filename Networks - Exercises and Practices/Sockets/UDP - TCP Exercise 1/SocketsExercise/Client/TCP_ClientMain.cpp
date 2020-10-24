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
PCSTR IP_Address = "127.0.0.1";
u_short ConnectionPort = 6060;

int main(int argc, char** argv)
{
	std::cout << "--- TCP CLIENT STARTUP ---" << std::endl << std::endl;

	// --- Initialize WinSock ---
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	// --- Create Connection Socket & Connect ---
	sockaddr_in connect_address;
	connect_address.sin_family = AF_INET;
	connect_address.sin_port = htons(ConnectionPort);
	inet_pton(AF_INET, IP_Address, &connect_address.sin_addr);

	SOCKET connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(connection_socket, (sockaddr*)&connect_address, sizeof(connect_address));

	// --- Send & Receive Messages ---
	std::string msg = "Client says: Ping";
	char buffer[1024]{ 0 };

	for (int i = 0; i < 5; ++i)
	{
		send(connection_socket, msg.c_str(), msg.size() + 1, 0);
		recv(connection_socket, buffer, sizeof(buffer), 0);
		std::cout << "[CLIENT]: Received Message (Iteration " << i << "):" << std::endl;
		std::cout << "\t" << buffer << std::endl << std::endl;
	}

	// --- Clean Up ---
	closesocket(connection_socket);
	WSACleanup();

	system("pause");
	return 0;
}