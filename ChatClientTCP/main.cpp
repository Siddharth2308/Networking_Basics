#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Initialize Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);
	if (wsok != 0)
	{
		std::cerr << "Can't Initialize Winsock !" << std::endl;
		return;
	}

	// Create a Socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);								// AF_INET cuz we using IPv4
	if (listening == INVALID_SOCKET)
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
	
	// Multiclient code below 
	fd_set master;
	FD_ZERO(&master);
	FD_SET(listening, &master);														// Adding Listening socket to master set

	while(true)
	{
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);				// Check MSDN select() function documentation for windows

		for(int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			
			if(sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				// Add the new connection to the list of connected clients i.e to the array set of master
				FD_SET(client, &master);
				
				// Send a welcome message to the connected client
				std::string welcomeMsg = "Welcome to E13menT's Chat Room !\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size()+1, 0);

//				char welcomeMsg[] = "Welcome to E13menT's Chat Room !";
//				send(client, welcomeMsg, sizeof(welcomeMsg) + 1, 0);

				// Broadcast we have a new connection.
			}
			else {
				char buf[4096];
				ZeroMemory(buf, 4096);
				int bytesIn = recv(sock, buf, 4096, 0);											// pass the desired socket to the recive function

				// Receive Message
				if(bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				} else
				{
					// Send the message to other connected clients except urself i.e listening
					for(int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if(outSock != listening && outSock != sock)
						{
							std::ostringstream ss;
							ss << "Socket #" << sock << ": " << buf << "\r\n";
							std::string strout = ss.str();

							send(outSock, strout.c_str(), strout.size() + 1, 0);
						}
					}
				}
			}
		}

	}

	// CleanUp Winsock
	WSACleanup();

	system("pause");
}