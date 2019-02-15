#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
#include <string>

void print(std::string msg, std::ofstream &ofs)
{
	std::cout << msg << std::endl;
	ofs << msg << std::endl;
}

int main()
{
	char filename[128] = {};
	std::cout << "Enter client filename: ";
	std::cin.getline(filename, sizeof(filename));
	std::ofstream ofs(filename);

	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		print("ERROR: Failed to start WSA", ofs);
		return -1;
	}

	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		print("ERROR:  Failed to create ClientSocket", ofs);
		return -1;
	}

	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address
	if ((connect(ClientSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		print("ERROR:  Connection attempted failed", ofs);
		return -1;
	}
		
	char * TxBuffer = nullptr;
	char RxBuffer[128] = {};
	
	int inputLength = 0;
	bool continueTransmit = true;
	bool loggedIn = false;
	while (loggedIn == false) {
		recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
		if (strcmp(RxBuffer, "Failed") == 0) {
			print(RxBuffer, ofs);
			loggedIn = true;
			continueTransmit = false;
		}
		else if (strcmp(RxBuffer, "Successful Login") == 0) {
			print(RxBuffer, ofs);
			loggedIn = true;
		}
		else {
			print(RxBuffer, ofs);
			std::string input;
			getline(std::cin, input);

			inputLength = input.size() + 1;
			TxBuffer = new char[inputLength];
			strcpy(TxBuffer, input.c_str());
			TxBuffer[input.length()] = '\0';

			send(ClientSocket, TxBuffer, inputLength, 0);
		}
	}

	while (continueTransmit) {
		std::string input;
		print("Enter a String to transmit", ofs);
		getline(std::cin, input);

		inputLength = input.size() + 1;
		TxBuffer = new char[inputLength];
		strcpy(TxBuffer, input.c_str());
		TxBuffer[input.length()] = '\0';

		print(TxBuffer, ofs);
		send(ClientSocket, TxBuffer, inputLength, 0);
		if (strcmp(TxBuffer, "Exit") == 0) {
			continueTransmit = false;
		}
	}

	closesocket(ClientSocket);
	WSACleanup();
	ofs.close();

	return 1;
}
