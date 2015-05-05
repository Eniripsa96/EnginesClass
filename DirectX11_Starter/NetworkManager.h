#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <stdio.h>
#include <queue>
#include <thread>

#pragma once

#define DEFAULT_IP "127.0.0.1"
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
	bool isSetUp();
	bool startListening(PCSTR ip = DEFAULT_IP, PCSTR port = DEFAULT_PORT);
	bool startServer();
	bool tryHost();
	bool tryConnect(PCSTR ip = DEFAULT_IP, PCSTR port = DEFAULT_PORT);
	void disconnect();
	bool emit(packetStruct* packet);
	bool hasData();
	packet getData();

	SOCKET Socket;
	bool connected;
	bool setUp;
	std::queue<packet> received;
private:
	WSADATA wsaData;
	std::thread* listenThread;
};
