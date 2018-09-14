#pragma once
#include "server.h"

Server::Server(void)
{	  // create WSADATA object

	WSADATA wsaData;

	// our sockets for the server

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;



	// address info for the server to listen to

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	// set address information

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!

	hints.ai_flags = AI_PASSIVE;
	// Resolve the server address and port

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Create a SOCKET for connecting to server

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}
	// Setup the TCP listening socket

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
	// no longer need address information

	freeaddrinfo(result);
	// start listening for new clients attempting to connect

	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}
bool Server::acceptNewClient(unsigned int & id)
{
	// if client waiting, accept the connection and save the socket

	ClientSocket = accept(ListenSocket, NULL, NULL);
	return false;
}


