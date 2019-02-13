#ifndef WINSOCK_H
#define WINSOCK_H

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

int const MAX_SOCKETS = 2;

class winsock {
protected:
	std::ofstream *ofs;
	int version_num1, version_num2; // determines the WSADATA version numbers
	int port; // port number
	std::string ip; //ip string e.g. "127.0.0.1"
	char rx_buffer[128]; //note that the rx_buffer has only 128 bytes
	WSADATA wsa_data;

	void Print(std::string msg)
	{
		std::cout << msg << std::endl;
		*ofs << msg << std::endl;
	}
public:
	void start_DLLS();
	SOCKET initialize_tcp_socket();
	winsock();
};

class winsock_server : public winsock {
protected:
	std::ofstream *ofs;
	std::string msgToWrite;
	SOCKET server_socket, connection_socket[MAX_SOCKETS + 1];
	int active_sockets[MAX_SOCKETS + 1];

	void Print(std::string msg)
	{
		std::cout << msg << std::endl;
		*ofs << msg << std::endl;
	}
public:
	void bind_socket();
	void listen_socket();
	int find_available_socket(void);
	int accept_connection();
	char * receive_message(int); //receives message from the connection_socket
	void send_message(char *, int); //sends message to the connection_socket
	void echo_mode(int); //prints a received message and sends it back
	winsock_server(int, std::string, std::ofstream *);
	~winsock_server();
};

class winsock_client : public winsock {
protected:
	SOCKET client_socket;
	std::ofstream *ofs;
	std::string msgToWrite;

	void Print(std::string msg)
	{
		std::cout << msg << std::endl;
		*ofs << msg << std::endl;
	}
public:
	char * receive_message(); //receives message from the client_socket
	void send_message(char *); //sends message to the client_socket
	void connect_to_tcp_server(); //tries to connect, exits if no server available
	void connect_to_tcp_server_loop(); //keeps trying to connect until successful
	winsock_client(int, std::string, std::ofstream *);
	~winsock_client();
};

#endif WINSOCK_H
