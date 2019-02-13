#include "oop_winsock.h"
#include <iostream>
#include <fstream>

char message_buffer[128];
bool buffer_full;

void Print(std::string msg, std::ofstream *ofs)
{
	std::cout << msg << std::endl;
	*ofs << msg << std::endl;
}

int main(int argc, char *argv[]) {
	std::string msgToWrite;
	std::ofstream ofs(argv[1]);
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open output file -- screen shots will be required" << std::endl;

	winsock_client client(27000, "127.0.0.1", &ofs);
	winsock_client clientReceive(27001, "127.0.0.1", &ofs);
	char rx_message[128] = "", tx_message[128] = "", clientName[32] = "", clientMessage[128] = "";

	client.connect_to_tcp_server_loop();

	Print("Enter User Name: ", &ofs);
	std::cin.getline(clientName, sizeof(clientName));

	strcpy(rx_message, client.receive_message());
	std::string s(rx_message, strlen(rx_message));
	msgToWrite = "server message:" + s;
	Print(msgToWrite, &ofs);

	if (strcmp(rx_message, "Server Full") == 0) {
		Print("Could not connect to server", &ofs);
		std::cin.get();
	} else {
		clientReceive.connect_to_tcp_server();
		strcpy(rx_message, clientReceive.receive_message());
		std::string s(rx_message, strlen(rx_message));
		if (strcmp(rx_message, "Server Full") == 0) {
			Print("Could not connect to server", &ofs);
			std::cin.get();
		}
		else {
			std::thread(&winsock_client::get_messages, &clientReceive).detach();
			while (true) {
				strcpy(tx_message,clientName);
				strcat(tx_message,": ");
				Print("Enter message to send: ", &ofs);
				std::cin.getline(clientMessage, sizeof(clientMessage));
				strcat(tx_message, clientMessage);
				if (strcmp(clientMessage, "quit") == 0) {
					client.send_message(clientMessage);
					break;
				}
				else {
					client.send_message(tx_message);
				}
			}
		}
	}

	WSACleanup();
	exit(0);
}