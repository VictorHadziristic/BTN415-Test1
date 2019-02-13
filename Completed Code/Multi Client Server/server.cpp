#include "oop_winsock.h"
#include <iostream>
#include <fstream>

extern int const MAX_SOCKETS;
char message_buffer[128] = {};
bool buffer_full = false;

int main() {
	std::ofstream ofs1("Server1_Output.txt");
	if (!ofs1.is_open())
		std::cout << "ERROR:  Failed to open output file -- Screenshots will be required" << std::endl;

	winsock_server server(27000, "127.0.0.1", &ofs1);
	winsock_server serverTransmit(27001, "127.0.0.1", &ofs1);
	int socket_number;
	int transmitSocket;
	int receiveSocket;

	while (true) {
		socket_number = server.accept_connection();
		if ( socket_number < MAX_SOCKETS) {
			receiveSocket = socket_number;
			socket_number = serverTransmit.accept_connection();
			if (socket_number < MAX_SOCKETS) {
				transmitSocket = socket_number;
				std::thread(&winsock_server::rec_mode, &server, receiveSocket).detach();
				std::thread(&winsock_server::send_mode, &serverTransmit, transmitSocket).detach();
			}
		}
	}

	WSACleanup();
	exit(0);
}
