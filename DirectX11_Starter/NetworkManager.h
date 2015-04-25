#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <stdio.h>
#include <queue>
#include <thread>

#pragma once

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

// Basic struct to extend when creating structs for data to send
struct packetStruct {};

struct packet {
	char* buffer;
	int length;
};

// Main class for managing the network connections
class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	bool isConnected();
	void startListening();
	void startServer();
	bool tryHost();
	bool tryConnect();
	void disconnect();
	bool emit(packetStruct* packet);

	SOCKET Socket = INVALID_SOCKET;
	bool connected = false;
	std::queue<packet> received;
private:
	WSADATA wsaData;
	std::thread* listenThread;
};
