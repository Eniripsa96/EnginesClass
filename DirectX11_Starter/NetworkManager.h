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

#define PACKET_NONE     -1
#define PACKET_PARTICLE 0
#define PACKET_JUDGES   1
#define PACKET_RESULT   2

// Basic struct to extend when creating structs for data to send
struct packetStruct {};

// Packet info used to keep track of buffer size and packet type
struct packet {
	char* buffer;
	int length;
	int type;
};

// Struct used for reading the packet type of any kind of packet
struct packetType {
	unsigned short type : 2;
};

// Packet for sharing a particle over the network
// Likely additions:
// - position
// - emit shape
struct particlePacket : packetStruct {

	particlePacket() : type(PACKET_PARTICLE) {}

	unsigned short type : 2;
	unsigned short amount : 14;
	unsigned short size : 8;
	unsigned short colorR : 8;
	unsigned short colorG : 8;
	unsigned short colorB : 8;
};

// Packet for sending judge data over the network
struct judgePacket : packetStruct {

	judgePacket() : type(PACKET_JUDGES) {}

	unsigned short type : 2;
	unsigned short extra : 6;
	unsigned short colorR1 : 8;
	unsigned short colorG1 : 8;
	unsigned short colorB1 : 8;
	unsigned short colorR2 : 8;
	unsigned short colorG2 : 8;
	unsigned short colorB2 : 8;
	unsigned short colorR3 : 8;
	unsigned short colorG3 : 8;
	unsigned short colorB3 : 8;
};

// Packet for sending a game result over the network
struct resultPacket : packetStruct {

	resultPacket() : type(PACKET_RESULT) {}

	unsigned short type : 2;
	unsigned short decision : 14;
};

// Main class for managing the network connections
class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	
	// Checks whether or not the network is connected to another user
	bool isConnected();

	// Checks whether or not the network data is set up and ready to be used
	bool isSetUp();

	// Starts listening to a hosting user at the given IP/Port combination.
	// If the IP and Port are not provided, this will listen on local host.
	// Returns whether or not the connection was successful.
	bool startListening(LPCSTR ip = DEFAULT_IP, PCSTR port = DEFAULT_PORT);

	// Starts a host setup to listen for other joining users
	// Returns whether or not the setup was successful.
	bool startServer();

	// Disconnects the user from it's current network communications.
	void disconnect();

	// Emits data over the network to other users.
	// If not currently connected, this will do nothing.
	bool emit(packetStruct* packet, int size);

	// Checks whether or not the network has received any data.
	bool hasData();

	// Retrieves the next packet received from the network in the queue to process.
	// This removes it from the queue on retrieval.
	packet getData();

	// Retrieves the type of the next packet received from the network.
	// If you use getData before this, it will not be the type of the same packet.
	// This will return PACKET_NONE if there are no pending packets.
	int getDataType();

	SOCKET Socket;
	bool connected;
	bool setUp;
	std::queue<packet> received;

private:
	
	// Attempts to set up a host setup for the network connections.
	// Returns whether or not it was successful.
	bool tryHost();

	// Attempts to connect to a hosting user at the given IP/Port combination.
	// Returns whether or not the connection was successful.
	bool tryConnect(LPCSTR ip, PCSTR port);

	WSADATA wsaData;
	std::thread* listenThread;
};
