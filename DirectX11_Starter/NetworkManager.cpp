#include "NetworkManager.h"

// Sets up the network connections, attempting to connect
// to the server.
NetworkManager::NetworkManager()
{
	setUp = false;
	connected = false;
	Socket = INVALID_SOCKET;
}

// Cleans up connections when destroyed if not done so elsewhere
NetworkManager::~NetworkManager()
{
	if (setUp)
	{
		closesocket(Socket);
		WSACleanup();
		if (listenThread) {
			listenThread->join();
		}
	}
}

// Checks whether or not the application is currently
// connected to another user and ready to play
bool NetworkManager::isConnected() {
	return connected;
}

// Checks whether or not the network communications
// are set up and ready to connect to another user
bool NetworkManager::isSetUp() {
	return setUp;
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

	setUp = true;
	return true;
}

// Tries to connect to the server. This can
// be used to reconnect after an error causes
// a disconnection or if it was disconnected
// manually.
bool NetworkManager::tryConnect(PCSTR ip, PCSTR port) {

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
	iResult = getaddrinfo(ip, port, &hints, &result);
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

	setUp = true;
	return true;
}

// Closes the network connection if it is
// currently connected.
void NetworkManager::disconnect() {
	if (setUp) {
		int iResult = shutdown(Socket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			closesocket(Socket);
			WSACleanup();
			setUp = false;
		}
	}
}

// The threaded method for listening to the hosting player as a client
void threadClientListen(NetworkManager* manager)
{
	char recvbuf[DEFAULT_BUFLEN];

	// Receive data until the server closes the connection
	int iResult;
	manager->connected = true;
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
	manager->connected = false;

	// Close the connection when finished if not already closed
	if (manager->setUp) {
		closesocket(manager->Socket);
		WSACleanup();
		manager->setUp = false;
	}
}

// Listens to the connection for incomming data. This
// repeats indefinitely so it should be started on
// a separate thread.
bool NetworkManager::startListening(PCSTR ip, PCSTR port)
{
	if (!setUp) {
		bool worked = tryConnect(ip, port);
		if (!worked) {
			return false;
		}
	}

	listenThread = new std::thread(threadClientListen, this);
	return true;
}

// Handles receiving clients and listening to each on a new thread
void threadServerHost(NetworkManager* manager)
{
	// Grab a client
	SOCKET client = accept(manager->Socket, NULL, NULL);
	if (client == INVALID_SOCKET) {
		manager->setUp = false;
		printf("accept failed 5d\n", WSAGetLastError());
		closesocket(manager->Socket);
		WSACleanup();
		return;
	}

	// Listen to the client
	char buffer[DEFAULT_BUFLEN];
	int iResult;
	manager->connected = true;
	do {
		iResult = recv(client, buffer, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			packet data = *new packet();
			data.buffer = new char[iResult];
			memcpy(data.buffer, buffer, iResult);
			data.length = iResult;

			packetType* info = (packetType*)data.buffer;
			data.type = info->type;

			manager->received.push(data);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("Receive failed: %d\n", WSAGetLastError());
			closesocket(client);
			WSACleanup();
			manager->setUp = false;
			manager->connected = false;
			return;
		}
	} while (iResult > 0);
	manager->connected = false;
}

// Starts the server, listening for new clients and receiving data
bool NetworkManager::startServer()
{
	if (!setUp) {
		bool worked = tryHost();
		if (!worked) {
			return false;
		}
	}

	listenThread = new std::thread(threadServerHost, this);
	return true;
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
		setUp = false;
		connected = false;
		return false;
	}

	// Send succeeded
	return true;
}

// Checks whether or not the network has pending data received
bool NetworkManager::hasData()
{
	return received.size() > 0;
}

// Gets the oldest pending data received from the network connection
packet NetworkManager::getData()
{
	packet data;
	data.buffer = 0;
	data.length = 0;

	if (hasData())
	{
		data = received.front();
		received.pop();
	}
	
	return data;
}

// Retrieves the type of packet of the next packet in the queue
// This returns PACKET_NONE if there isn't any pending packets
int NetworkManager::getDataType()
{
	if (hasData())
	{
		return received.front().type;
	}
	else return PACKET_NONE;
}