#include <windows.networking.sockets.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void main()
{
	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;

	//create server socket
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return;
	}

	//binds socket to address
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	//listen on a socket
	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}


	cout << "Waiting for client connection\n" << endl;

	//accepts a connection from a client
	SOCKET ConnectionSocket;
	ConnectionSocket = SOCKET_ERROR;
	if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	cout << "Connection Established" << endl;

	while (1) {
		//receives RxBuffer
		char RxBuffer[128] = {};
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
		cout << "Msg Rx: " << RxBuffer << endl;
	}
	closesocket(ConnectionSocket);	//closes incoming socket
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources
}