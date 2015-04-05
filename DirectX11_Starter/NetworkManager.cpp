#include "NetworkManager.h"

// Sets up the network connections, attempting to connect
// to the server.
NetworkManager::NetworkManager()
{
	
}

// Cleans up connections when destroyed if not done so elsewhere
NetworkManager::~NetworkManager()
{
	
}

// Checks whether or not the application is currently
// connected to the server and ready for network communications.
bool NetworkManager::isConnected() {
	return connected;
}

// Attempts to set up a server to host other players
bool NetworkManager::tryHost() {

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}

	// addrinfo structs to use
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Assemble the host info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Try to create the info representation
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Try to set up the socket
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Bind the socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
	freeaddrinfo(result);

	// Mark the socket as listening for new clients
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listn failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	connected = true;
}

// Tries to connect to the server. This can
// be used to reconnect after an error causes
// a disconnection or if it was disconnected
// manually.
bool NetworkManager::tryConnect() {

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}

	// addrinfo structs to use
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Reset the data for the template
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
		return false;
	}

	// Try to initialize the socket
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}
	freeaddrinfo(result);

	// Try to connect to the server
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Validate the connection
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to the server!\n");
		WSACleanup();
		return false;
	}

	connected = true;
	return true;
}

// Closes the network connection if it is
// currently connected.
void NetworkManager::disconnect() {
	if (connected) {
		int iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			WSACleanup();
			connected = false;
		}
	}
}

// Listens to the connection for incomming data. This
// repeats indefinitely so it should be started on
// a separate thread.
void NetworkManager::startListening()
{
	if (!connected) return;

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

	// Close the connection when finished if not already closed
	if (connected) {
		closesocket(ConnectSocket);
		WSACleanup();
		connected = false;
	}
}

// Starts the server, listening for new clients and receiving data
void NetworkManager::startServer() 
{
	if (!connected) return;
}

// Sends data over the network if currently connected
bool NetworkManager::emit(packetStruct* packet) {
	if (!connected) return false;

	// Attempt to send the data
	int iResult = send(ConnectSocket, (char*)packet, sizeof(packet), 0);

	// If there was an error, print the message
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		connected = false;
		return false;
	}

	// Send succeeded
	return true;
}