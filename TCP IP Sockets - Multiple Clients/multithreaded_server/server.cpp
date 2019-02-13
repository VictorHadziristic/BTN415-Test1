#include "oop_winsock.h"
#include <iostream>
#include <fstream>

extern int const MAX_SOCKETS;

int main() {
	std::ofstream ofs1("Server1_Output.txt");
	if (!ofs1.is_open())
		std::cout << "ERROR:  Failed to open output file -- Screenshots will be required" << std::endl;

	winsock_server server(27000, "127.0.0.1", &ofs1);
	int socket_number;

	while (true) {
		socket_number = server.accept_connection();
		if ( socket_number < MAX_SOCKETS) {
			std::thread(&winsock_server::echo_mode, &server, socket_number).detach();
		}
	}

	WSACleanup();
	exit(0);
}
