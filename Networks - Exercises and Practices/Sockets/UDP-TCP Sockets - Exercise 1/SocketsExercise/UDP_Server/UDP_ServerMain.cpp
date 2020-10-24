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
PCSTR Dest_IP_Address = "127.0.0.1";
PCSTR Source_IP_Address = "127.0.0.1";
u_short ConnectionPort = 8000;

int main(int argc, char** argv)
{
	std::cout << "--- UDP SERVER STARTUP ---" << std::endl << std::endl;
	
	// --- Initialize WinSock ---
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	// --- Setup Addresses (local & dest) ---
	sockaddr_in local_address;
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(ConnectionPort);
	inet_pton(AF_INET, Source_IP_Address, &local_address.sin_addr.S_un.S_addr); //or local_address.sin_addr.S_un.S_addr = INADDR_ANY; (??? --> Didn't tested it)

	sockaddr_in dest_address;
	dest_address.sin_family = AF_INET;
	dest_address.sin_port = htons(ConnectionPort);
	inet_pton(AF_INET, Dest_IP_Address, &dest_address.sin_addr.S_un.S_addr);

	// --- Create Socket (UDP) & Bind ---
	SOCKET UDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind(UDPSocket, (sockaddr*)&local_address, sizeof(local_address));

	// --- Receive Message ---
	std::string msg = "Server Says: Pong";
	char buffer[1024]{ 0 };
	int sender_address_size = sizeof(local_address);

	for (int i = 0; i < 5; ++i)
	{		
		recvfrom(UDPSocket, buffer, sizeof(buffer), 0, (sockaddr*)&local_address, &sender_address_size);
		sendto(UDPSocket, msg.c_str(), msg.size() + 1, 0, (sockaddr*)&dest_address, sizeof(dest_address));
		std::cout << "[SERVER]: Received Message (Iteration " << i << "):" << std::endl;
		std::cout << "\t" << buffer << std::endl << std::endl;
	}

	// --- Pause (before clean!) ---
	system("pause");
	// --- Clean Up ---
	closesocket(UDPSocket);
	WSACleanup();
	return 0;
}