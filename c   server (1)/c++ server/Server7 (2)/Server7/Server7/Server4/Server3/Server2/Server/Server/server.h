#pragma once
#include <winsock2.h>
#include <Windows.h>

#include <ws2tcpip.h>
#include <stdio.h> 
// size of our buffer

#define DEFAULT_BUFLEN 512
// port to connect sockets through 

#define DEFAULT_PORT "27015"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
class Server
{

public:

	// accept new connections
	bool acceptNewClient(unsigned int & id);

	// Socket to listen for new connections

	SOCKET ListenSocket;

	// Socket to give to the clients

	SOCKET ClientSocket;

	// for error checking return values

	int iResult;

	Server(void);
	~Server(void);
};