#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Initialize Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);
	if(wsok != 0)
	{
		std::cerr << "Can't Initialize Winsock !" << std::endl;
		return;
	}

	// Create a Socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);								// AF_INET cuz we using IPv4
	if(listening == INVALID_SOCKET)
	{
		std::cerr << "Can't Create the requested Socket ! " << std::endl;
		return;
	}
	
	// Bind the socket to and IP and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;														// We using IPv4
	hint.sin_port = htons(5400);													// htons == host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY;											// Can also use inet_pton ...
	bind(listening, (sockaddr*)&hint, sizeof(hint));
	
	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);													// SOMAXCONN is max amout of connections as backlog to stack
	std::cout << "Initialization Complete Waiting for connection ..." << std::endl;

	// Wait for connection
	sockaddr_in client;
	int ClientSize = sizeof(client);

	SOCKET clientsocket = accept(listening, (sockaddr*)&client, &ClientSize);
	if(clientsocket == INVALID_SOCKET)
	{
		std::cerr << "Invalid Client Socket !" << std::endl;
		return;
	}

	char host[NI_MAXHOST];															// CLient's remote name
	char service[NI_MAXSERV];														// Service (i.e port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);													// Same as memset(host, 0, NI_MAXHOST) --> for linux and MAC or basically UNIX
	ZeroMemory(service, NI_MAXSERV);												// ZeroMemory is a function specific to windows

			
	if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << "Connected on Port :" << service << std::endl;
	} 
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << "Connected on Port :" << ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	closesocket(listening);

	// Accept and Echo Message Back to Client
	char buf[4096];

	while(true)
	{
		ZeroMemory(buf, 4096);
		// Wait for client to send data
		int bytesReceived = recv(clientsocket, buf, 4096, 0);
		if(bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). " << std::endl;
			break;
		}
		if(bytesReceived  == 0)
		{
			std::cout << "Client Disconnected" << std::endl;
			break;
		}
		// Echo message back to client
		send(clientsocket, buf, bytesReceived + 1, 0);
	}
	// Close the Socket
	closesocket(clientsocket);
	// CleanUp Winsock
	WSACleanup();
}