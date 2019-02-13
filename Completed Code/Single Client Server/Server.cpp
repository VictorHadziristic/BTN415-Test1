//   /$$$$$$$$/$$$$$$$$  /$$$$$$  /$$   /$$ /$$   /$$  /$$$$$$        /$$   /$$ /$$       /$$$$$$$  /$$$$$$$$ /$$$$$$$  /$$$$$$$  /$$   /$$ /$$       /$$      
//  |__  $$__/ $$_____/ /$$__  $$| $$  | $$| $$$ | $$ /$$__  $$      | $$$ | $$| $/      | $$__  $$| $$_____/| $$__  $$| $$__  $$| $$  | $$| $$      | $$      
//     | $$  | $$      | $$  \__/| $$  | $$| $$$$| $$| $$  \ $$      | $$$$| $$|_/       | $$  \ $$| $$      | $$  \ $$| $$  \ $$| $$  | $$| $$      | $$      
//     | $$  | $$$$$   | $$      | $$$$$$$$| $$ $$ $$| $$  | $$      | $$ $$ $$          | $$$$$$$/| $$$$$   | $$  | $$| $$$$$$$ | $$  | $$| $$      | $$      
//     | $$  | $$__/   | $$      | $$__  $$| $$  $$$$| $$  | $$      | $$  $$$$          | $$__  $$| $$__/   | $$  | $$| $$__  $$| $$  | $$| $$      | $$      
//     | $$  | $$      | $$    $$| $$  | $$| $$\  $$$| $$  | $$      | $$\  $$$          | $$  \ $$| $$      | $$  | $$| $$  \ $$| $$  | $$| $$      | $$      
//     | $$  | $$$$$$$$|  $$$$$$/| $$  | $$| $$ \  $$|  $$$$$$/      | $$ \  $$          | $$  | $$| $$$$$$$$| $$$$$$$/| $$$$$$$/|  $$$$$$/| $$$$$$$$| $$$$$$$$
//     |__/  |________/ \______/ |__/  |__/|__/  \__/ \______/       |__/  \__/          |__/  |__/|________/|_______/ |_______/  \______/ |________/|________/
//     By: Victor Hadziristic                                                                                                                                                        
//     Class: BTN415                                                                                                                                          
//     Lab 1 (At Home) Server.cpp
#include <windows.networking.sockets.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
using namespace std;

void main()
{
	std::ofstream ofs("Output.txt");

	if (!ofs)
		std::cout << "ERROR: Failed to open output file" << std::endl;

	bool open = false;
	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ofs << "ERROR:  Failed to start WSA" << std::endl;
		return;
	}

	//create server socket
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		open = false;
		WSACleanup();
		ofs << "ERROR:  Failed to create ServerSocket" << std::endl;
		return;
	}
	else {
		open = true;
	}

	//binds socket to address
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		open = false;
		closesocket(ServerSocket);
		WSACleanup();
		ofs << "ERROR:  Failed to bind ServerSocket" << std::endl;
		return;
	}
	else {
		open = true;
	}

	//listen on a socket
	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		open = false;
		closesocket(ServerSocket);
		WSACleanup();
		ofs << "ERROR:  listen failed to configure ServerSocket" << std::endl;
		return;
	}
	else {
		open = true;
	}

	SOCKET ConnectionSocket;
	//Poor design but for the sake of this lab I want the server to continually look for incoming connections
	while (true) {
		ofs << "Waiting for client connection\n" << endl;

		//accepts a connection from a client
		ConnectionSocket = SOCKET_ERROR;
		if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
			open = false;
			closesocket(ServerSocket);
			WSACleanup();
			return;
		}
		else {
			open = true;
		}

		ofs << "Connection Established" << endl;

		while (open) {
			//receives RxBuffer
			char RxBuffer[128] = {};
			recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			if (strcmp(RxBuffer, "Quit") == 0 || strcmp(RxBuffer, "quit") == 0) {
				RxBuffer[0] = '\0';
				ofs << endl;
				open = false;
				closesocket(ConnectionSocket);	//closes incoming socket
				closesocket(ServerSocket);	    //closes server socket	
				WSACleanup();					//frees Winsock resources
			}
			else {
				ofs << RxBuffer;
				send(ConnectionSocket, "Received Message", sizeof("Received Message"), 0);
			}
		}
	}
}