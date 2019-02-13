#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
using namespace std;

void print(std::ostream& os, const char* msg)
{
	os << msg << std::endl;
	std::cout << msg << std::endl;
}

int main()
{
	std::ofstream ofs("Client_Output.txt");
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open the output file" << std::endl;

	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		print(ofs, "ERROR: Failed to start WSA");
		return 0;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		print(ofs, "ERROR:  Failed to create ClientSocket");
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
		print(ofs, "ERROR:  Connection attempted failed");
		return 0;
	}


	//receives Rxbuffer

	while (1) {
		//sends Txbuffer		
		char TxBuffer[128] = {};
		print(ofs, "Enter a String to transmit");
		cin >> TxBuffer;
		std::string msg = "Tx: ";
		msg += TxBuffer;
		print(ofs, msg.c_str());
		send(ClientSocket, TxBuffer, sizeof(TxBuffer), 0);
	}

	//closes connection and socket
	closesocket(ClientSocket);
	//frees Winsock DLL resources
	WSACleanup();

	return 1;
}
