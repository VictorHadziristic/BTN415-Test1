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
//     Lab 1 (At Home) Client.cpp
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
using namespace std;

void main()
{
	std::ofstream ofs("Output.txt");

	bool open = false;
	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		ofs << "ERROR: Failed to start WSA" << std::endl;
		return;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		open = false;
		WSACleanup();
		ofs << "ERROR:  Failed to create ClientSocket" << std::endl;
		return;
	}
	else {
		open = true;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address
	if ((connect(ClientSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		open = false;
		closesocket(ClientSocket);
		WSACleanup();
		ofs << "ERROR:  Connection attempted failed" << std::endl;
		return;
	}
	else {
		open = true;
	}
	
	std::ifstream ifs("Test.txt");
	char RxBuffer[128] = {};
	//sends Txbuffer
	char current;
	char TxBuffer[1];

	while (open) {
		fstream fin("Test.txt", fstream::in);
		while (fin >> noskipws >> current) {
			TxBuffer[0] = current;
			send(ClientSocket, TxBuffer, sizeof(TxBuffer), 0);
			recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
			if (strcmp(RxBuffer, "") != 0) {
				ofs << RxBuffer << endl;
			}
		}
		//closing up used files
		ofs.close();
		ifs.close();
		//Signify to server that transmission has ended
		send(ClientSocket, "quit", sizeof("quit"), 0);
		//closes connection and socket
		closesocket(ClientSocket);
		//frees Winsock DLL resources
		WSACleanup();
		open = false;
		exit(0);
	}
}
