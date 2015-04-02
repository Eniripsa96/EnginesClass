#include "NetworkManager.h"


NetworkManager::NetworkManager()
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return;
	}

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	
	// Protocol type TCP/UDP
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_protocol = IPPROTO_UDP;

	// Try to grab the server information
	iResult = getaddrinfo("address", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("aetaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Try to initialize the socket
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Try to connect to the server
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Validate the connection
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to the server!\n");
		WSACleanup();
		return;
	}

	valid = true;
}


NetworkManager::~NetworkManager()
{
}

// Checks whether or not the NetworkManager
// has been successfully set up. If this is
// false, no networking will be possible.
bool NetworkManager::isValid()
{
	return valid;
}

// Listens to the connection for incomming data. This
// repeats indefinitely so it should be started on
// a separate thread.
void NetworkManager::listen()
{
	char recvbuf[DEFAULT_BUFLEN];

	// Receive data until the server closes the connection
	int iResult;
	do {
		iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} 
	while (iResult > 0);
}