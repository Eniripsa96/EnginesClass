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
	Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Bind the socket
	iResult = bind(Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(Socket);
		WSACleanup();
		return false;
	}
	freeaddrinfo(result);

	// Mark the socket as listening for new clients
	if (listen(Socket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listn failed with error: %ld\n", WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		return false;
	}

	connected = true;
	return true;
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
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("aetaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Try to initialize the socket
	ptr = result;
	Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (Socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Try to connect to the server
	iResult = connect(Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(Socket);
		Socket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	// Validate the connection
	if (Socket == INVALID_SOCKET) {
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
		int iResult = shutdown(Socket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			closesocket(Socket);
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

	listenThread = std::thread(threadClientListen, this);
}

void threadClientListen(NetworkManager* manager)
{
	char recvbuf[DEFAULT_BUFLEN];

	// Receive data until the server closes the connection
	int iResult;
	do {
		iResult = recv(manager->Socket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			packet data = *new packet();
			data.buffer = new char[iResult];
			memcpy(data.buffer, recvbuf, iResult);
			data.length = iResult;

			manager->received.push(data);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);

	// Close the connection when finished if not already closed
	if (manager->connected) {
		closesocket(manager->Socket);
		WSACleanup();
		manager->connected = false;
	}
}

// Starts the server, listening for new clients and receiving data
void NetworkManager::startServer()
{
	if (!connected) return;

	listenThread = std::thread(threadServerHost, this);
}

// Handles receiving clients and listening to each on a new thread
void threadServerHost(NetworkManager* manager)
{
	// Grab a client
	SOCKET client = accept(manager->Socket, NULL, NULL);
	if (client == INVALID_SOCKET) {
		manager->connected = false;
		printf("accept failed 5d\n", WSAGetLastError());
		closesocket(manager->Socket);
		WSACleanup();
		return;
	}

	// Listen to the client
	char buffer[DEFAULT_BUFLEN];
	int iResult;
	do {
		iResult = recv(client, buffer, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			packet data = *new packet();
			data.buffer = new char[iResult];
			memcpy(data.buffer, buffer, iResult);
			data.length = iResult;
			
			manager->received.push(data);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("Receive failed: %d\n", WSAGetLastError());
			closesocket(client);
			WSACleanup();
			return;
		}
	} while (iResult > 0);
}

// Sends data over the network if currently connected
bool NetworkManager::emit(packetStruct* packet) {
	if (!connected) return false;

	// Attempt to send the data
	int iResult = send(Socket, (char*)packet, sizeof(packet), 0);

	// If there was an error, print the message
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		connected = false;
		return false;
	}

	// Send succeeded
	return true;
}