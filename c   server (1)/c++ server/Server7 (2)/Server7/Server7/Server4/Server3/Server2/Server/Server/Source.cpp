#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex>
#include <iostream>
#include <string>
#include "game.h"
#include <fstream>
#include <vector>
#include <ctime>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1 "
std::mutex MyMutex;
int ClientCount;
using namespace std;
int specIndex;
int gameQueue[5] = {-1,-1,-1,-1,-1};
vector<int> specQueue;
std::ofstream outfile ("log.txt", std::ofstream::out | std::ofstream::app);

template<typename T>
void pop_front(std::vector<T>& vec)
{
	vec.front() = vec.back();
	vec.pop_back();
}

int callThread(SOCKET ClientSocket, int ID,game game1)
{
	
	ClientCount++;
	char recvbuf[DEFAULT_BUFLEN];
	string type = game1.getType();
	char* total = (char*)malloc(100);
	char* total2 = (char*)malloc(100);
	char* total3 = (char*)malloc(100);
	const char *join = "joining ";
	const char *spec = "spectating ";
	const char *temp = type.c_str();
	const char *change = "Changing to game mode and joining ";
	strcpy(total, join);
	strcat(total, temp);

	strcpy(total2, spec);
	strcat(total2, temp);

	strcpy(total3, change);
	strcat(total3, temp);
	int index = -1;
	int check = 0;
	int tempSpec = -1;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult, iSendResult;
	int gameCount = 0;
	// Receive until the peer shuts down the connection
		
			do {
					iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
					cout << ClientCount << endl;
					if (ClientCount > 1) {
						for (int i = 0; i < 5; i++)
						{	//if id is in gamequeue break
							if (gameQueue[i] == ID)
							{
								check = 1;
								break;
							}
							if (gameQueue[i] == -1)
							{
								gameCount++;
							}

						}
						//if not in game queue add to game queue
						for (int i = 0; i < 5; i++)
						{
							if (gameQueue[i] == -1 && check != 1 && check != 2)
							{
								gameQueue[i] = ID;
								send(ClientSocket, total, strlen(total), 0);
								index = i;
								check = 1;
								break;
							}
						}
						//if game is full add to spectator queue
						if (check == 0)
						{
							specQueue.push_back(ID);
							send(ClientSocket, total2, strlen(total2), 0);
							check = 2;
						}
						//if it is in spec QUeue
						if (check == 2)
						{
							if (specQueue[0] == -1)
							{

								pop_front(specQueue);
							}

							if (gameCount > 0 && specQueue[0] == ID)
							{
								send(ClientSocket, total3, strlen(total3), 0);
								pop_front(specQueue);
								check = 0;
								cout << "popped" << endl;
								for (int i = 0; i < 5; i++)
								{	//if id is in gamequeue break
									if (gameQueue[i] == ID)
									{
										check = 1;
										break;
									}
									for (int i = 0; i < 5; i++)
									{
										if (gameQueue[i] == -1 && check != 1)
										{
											gameQueue[i] = ID;
											send(ClientSocket, total, strlen(total), 0);
											index = i;
											check = 1;
											break;
										}
									}

								}
							}
						}
					}
					
					if (iResult > 0) {
					
						cout << "Bytes received: " << iResult << endl;
						
							for (int i = 0; i < iResult ; i++)
							{
								cout << recvbuf[i];
							}	
							// Echo the buffer back to the sender
							
							iSendResult = send(ClientSocket, recvbuf, iResult, 0);
						
							if (iSendResult == SOCKET_ERROR) {
								
									cout << "send failed:" <<  WSAGetLastError() << endl;
									MyMutex.lock();
									ClientCount--;
									MyMutex.unlock();
									closesocket(ClientSocket); WSACleanup();
									free(total);
									free(total2);
									free(total3);
									return 1;
									ExitThread;
							}
						
							cout << "Bytes sent: " << iSendResult << endl;
						
					}
				
					else if (iResult == 0)
					{
						if (index != -1)
						{
							gameQueue[index] = -1;
						}
						if (tempSpec > -1)
						{
							specQueue[tempSpec - 1] = -1;
						}
						MyMutex.lock();
						ClientCount--;
						MyMutex.unlock();
						closesocket(ClientSocket);
						cout << "Connection closing..." << endl;
						free(total);
						free(total2);
						free(total3);
						ExitThread;
						
					}
					else {
					
						cout << "recv failed: " << WSAGetLastError() << endl;
						if (index != -1)
						{
							gameQueue[index] = -1;
						}
						if (tempSpec > -1)
						{
							specQueue[tempSpec - 1] = -1;
						}
						MyMutex.lock();
						ClientCount--;
						MyMutex.unlock();
						closesocket(ClientSocket);
						free(total);
						free(total2);
						free(total3);
						return 1;
						ExitThread;
				}
					if (recvbuf[0] == 'y')
					{
						MyMutex.lock();
						cout << "Client" << ID << "has quit" << endl;
						if (index != -1)
						{
							gameQueue[index] = -1;
						}
						if (tempSpec > -1)
						{
							specQueue[tempSpec - 1] = -1;
						}
						ClientCount--;
						MyMutex.unlock();
						closesocket(ClientSocket);
						iResult = 0;
						free(total);
						free(total2);
						free(total3);
						ExitThread;
					}
			} while (iResult > 0);

			
}


int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int threadCount = 0;
	int iSendResult;
	ClientCount = 0;
	int ClientID = 0;
	time_t t = time(0);
	char* dt = ctime(&t);
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	std::thread myThreads[100];
	game game1;
	int type;
	string map;
	int level;
	int max;
	int min;
	//output server details to file 
	ofstream outfile;
	outfile.open("log.txt");
	outfile << DEFAULT_IP;
	outfile << DEFAULT_PORT;
	
	//get start up data
	cout << "please enter the number for game type: 1: Deathmatch 2: Capture the flag 3: Blood Diamond" << endl;
	cin >> type;
	if (type == 1)
	{
		game1.setType("deathmatch");
	}
	if (type == 2)
	{
		game1.setType("Capture the flag");

	}
	if (type == 3)
	{
		game1.setType("Blood Diamond");
	}
	cout << "Please choose a map:" << endl;
	cin >> map;
	cout << "please choose a difficulty level between 1 - 3: " << endl;
	cin >> level;
	

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
	cout <<"getaddrinfo failed with error:" <<  iResult << endl;
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	freeaddrinfo(result);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	for (;;)
	{

	

		
		//creating a temp socket for accepting a connction
		SOCKET ClientSocket;
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			cout << "accept failed: " << WSAGetLastError() << endl;
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		else
		{
			
			
			myThreads[threadCount] = std::thread(callThread,ClientSocket,ClientID,game1);
			outfile << "Client" << ClientID << "joined at: " << dt << endl;
			threadCount++;
			ClientID++;
			if (ClientCount == 1)
			{
				cout << "Waiting for another Client to connect" << endl;
			}
			if (ClientCount >= 2)
			{
				cout << "Game" << type << "in progress" << endl;
			}
			outfile << "Total number of clients: " << ClientID << endl;
		}
	}
	
	outfile.close();
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed with error: " << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	system("PAUSE");
	return 0;
}