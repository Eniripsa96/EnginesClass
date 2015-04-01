#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <stdio.h>

#pragma once

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	bool isValid();
private:
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	bool valid = false;
	bool connected;
};

