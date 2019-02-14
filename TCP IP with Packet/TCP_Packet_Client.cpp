#include <windows.networking.sockets.h>
#include "tcpPacket.h"
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void print(std::ostream& os, const char* msg)
{
	os << msg << std::endl;
	std::cout << msg << std::endl;
}

int main()
{
	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address
	if ((connect(ClientSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		return 0;
	}


	//receives Rxbuffer
	char * TxBuffer = nullptr;
	while (1) {
		//Create a new packet and find out the size of it when it is empty
		Packet packet = Packet();
		unsigned int EmptyPktSize = 1;
		string input;
		unsigned int payload;

		//Handling Input
		std::cout << "Enter string to transmit" << endl;
		getline(std::cin,input);
		payload = input.size() + 1;
		packet.Data = new char[payload];
		strcpy(packet.Data, input.c_str());
		packet.Data[input.length()] = '\0';

		//Store the size of data in the header
		packet.Head.numOfBytes = payload;

		//Serialization
		if (TxBuffer) { delete[] TxBuffer; }
		unsigned int TotalSize = EmptyPktSize + packet.Head.numOfBytes;
		TxBuffer = new char[TotalSize];
		memcpy(TxBuffer, &packet.Head, 1);
		memcpy(TxBuffer + 1, packet.Data, packet.Head.numOfBytes);
		cout << TxBuffer << endl;
		send(ClientSocket, TxBuffer, TotalSize, 0);
	}

	//closes connection and socket
	closesocket(ClientSocket);
	//frees Winsock DLL resources
	WSACleanup();

	return 1;
}
