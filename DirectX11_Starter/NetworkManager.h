#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <stdio.h>
#include <queue>

#pragma once

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

// Basic struct to extend when creating structs for data to send
struct packetStruct {};

// Main class for managing the network connections
class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	bool isConnected();
	void listen();
	bool tryConnect();
	void disconnect();
	bool emit(packetStruct* packet);
private:
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	bool connected = false;
};
