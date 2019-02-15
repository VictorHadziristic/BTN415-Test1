#include <windows.networking.sockets.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

void print(std::string msg, std::ofstream &ofs)
{
	std::cout << msg << std::endl;
	ofs << msg << std::endl;
}

void threadLogic(SOCKET ConnectionSocket, std::ofstream& ofs) {
	print("Thread Started", ofs);
	char RxBuffer[128] = {};
	bool exit = false;
	while (exit == false) {
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
		if (strcmp(RxBuffer, "Exit") == 0) {
			exit = true;
			closesocket(ConnectionSocket);
		}
		else {
			send(ConnectionSocket, "Received", sizeof("Received"), 0);
			std::string MsgToSend = "Msg Rx: " + (std::string)RxBuffer;
			print(MsgToSend, ofs);
		}
	}
	print("Thread Ended", ofs);
}

int main()
{
	std::ofstream ofs("Server_Output.txt");
	if (!ofs)
		std::cout << "ERROR: Failed to open output file" << std::endl;
	

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		print("ERROR:  Failed to start WSA", ofs);
		return -1;
	}

	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		print("ERROR:  Failed to create ServerSocket", ofs);
		return -1;
	}

	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		print("ERROR:  Failed to bind ServerSocket", ofs);
		return -1;
	}

	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		print("ERROR:  listen failed to configure ServerSocket", ofs);
		return -1;
	}


	print("Waiting for client connection", ofs);

	while (true) {
		SOCKET ConnectionSocket;
		ConnectionSocket = SOCKET_ERROR;
		if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
			closesocket(ServerSocket);
			WSACleanup();
			return -1;
		}

		print("Connection Established", ofs);

		int tries = 0;
		bool loggedIn = false;
		bool valid = false;

		char RxBuffer[128] = {};
		send(ConnectionSocket, "Username: ", sizeof("Username: "), 0);
		while (!loggedIn) {
			recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			if (strcmp(RxBuffer, "BTNGuest") == 0) {
				send(ConnectionSocket, "Successful Login", sizeof("Successful Login"), 0);
				loggedIn = true;
				valid = true;
				std::thread(threadLogic,std::move(ConnectionSocket),std::ref(ofs)).detach();
			}
			else {
				tries += 1;
				if (tries <= 2) {
					send(ConnectionSocket, "Try Again", sizeof("Try Again"), 0);
				}
				else {
					send(ConnectionSocket, "Failed", sizeof("Failed"), 0);
					loggedIn = true;
				}

			}
		}
	}

	closesocket(ServerSocket);	   
	WSACleanup();
	ofs.close();

	return 1;
}