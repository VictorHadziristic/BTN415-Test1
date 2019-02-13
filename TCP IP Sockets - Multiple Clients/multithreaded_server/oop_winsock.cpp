#include "oop_winsock.h"

//starts all necessary Windwos dlls
void winsock::start_DLLS() {
	if ((WSAStartup(MAKEWORD(this->version_num1, this->version_num2), &this->wsa_data)) != 0) {
		Print("Could not start DLLs");
		std::cin.get();
		exit(0);
	}
}

//initializes a socket and returns it
SOCKET winsock::initialize_tcp_socket() {
	SOCKET LocalSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (LocalSocket == INVALID_SOCKET) {
		WSACleanup();
		Print("Could not initialize socket");
		std::cin.get();
		exit(0);
	}
	return LocalSocket;
}

//base constructor (called for all objects)
//note that in case multiple socket objects are created, start_DLLS()
//is called multiple times. This, however, does not result in errors
//just a slight redundancy
winsock::winsock(){
	this->version_num1 = 2;
	this->version_num2 = 2;
	this->start_DLLS();
}

//binds the server socket
void winsock_server::bind_socket(){
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET; //Address family type internet
	SvrAddr.sin_port = htons(this->port); //port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr(this->ip.c_str()); //IP address
	if ((bind(this->server_socket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(this->server_socket);
		WSACleanup();
		Print("Could not bind to the socket");
		std::cin.get();
		exit(0);
	}
}

//puts the server socket in listening mode
void winsock_server::listen_socket(){
	if (listen(this->server_socket, 1) == SOCKET_ERROR) {
		closesocket(this->server_socket);
		WSACleanup();
		Print("Could not listen to the provided socket.");
		std::cin.get();
		exit(0);
	}
	else {
		Print("Waiting for client connection");
	}
}

int winsock_server::find_available_socket(void) {
	for (int socket_number = 0; socket_number < MAX_SOCKETS; socket_number++) {
		if (this->active_sockets[socket_number] == 0) {
			return socket_number;
		}
	}
	return MAX_SOCKETS;
}

void winsock_server::echo_mode(int socket_number) {
	msgToWrite = "Thread Started for Socket " + socket_number;
	Print(msgToWrite);
	char rx_buffer[128] = {};
	while (true) {
		strcpy(rx_buffer, this->receive_message(socket_number));

		std::string s(rx_buffer, strlen(rx_buffer));
		msgToWrite = "From connection " + socket_number + s;
		Print(msgToWrite);
		this->send_message("message received",socket_number);
		if (strcmp(rx_buffer, "quit") == 0) {
			msgToWrite = "Closing Connection with " + socket_number;
			Print(msgToWrite);
			closesocket(this->connection_socket[socket_number]);
			this->active_sockets[socket_number] = 0;
			break;
		}
	}	
}

//accepts incoming connections
int winsock_server::accept_connection(){
	SOCKET aux_connection_socket;
	int socket_number = MAX_SOCKETS;

	if ((aux_connection_socket = accept(this->server_socket, NULL, NULL)) == SOCKET_ERROR) {
		Print("Could not accept incoming connection");
		std::cin.get();
	} else {
		socket_number = this->find_available_socket(); 
		if (socket_number < MAX_SOCKETS){
			Print("Connection Accepted");
			this->connection_socket[socket_number] = aux_connection_socket;
			this->send_message("Welcome", socket_number);
			this->active_sockets[socket_number] = 1;
		}
		else {
			this->connection_socket[socket_number] = aux_connection_socket;
			this->send_message("Server Full", socket_number);
			closesocket(this->connection_socket[socket_number]);
			Print("Someone tried to connect when I was full");
		}
	}
	return socket_number;
}

//receives messages from the connection_socket
char * winsock_server::receive_message(int index) {
	recv(this->connection_socket[index], this->rx_buffer, sizeof(this->rx_buffer), 0);
	return this->rx_buffer;
}

//sends messages to the connection_socket
void winsock_server::send_message(char * tx_buffer, int index) {
	send(this->connection_socket[index], tx_buffer, strlen(tx_buffer), 0);
}

//server constructor that sets up the port number and ip number
//it also initializes the socket, binds it, and puts it in listening mode
winsock_server::winsock_server(int port, std::string ip, std::ofstream *theStream){
	this->ofs = theStream;
	this->port = port;
	this->ip = ip; 
	this->server_socket = this->initialize_tcp_socket();
	this->bind_socket();
	this->listen_socket();
	for (int socket_number = 0; socket_number <= MAX_SOCKETS; socket_number++){
		this->active_sockets[socket_number] = 0;
	}
}

//server destructor that closes all sockets opened by this object
winsock_server::~winsock_server(){
	for (int socket_number = 0; socket_number < MAX_SOCKETS; socket_number++) {
		if (this->active_sockets[socket_number] == 1){
			closesocket(this->connection_socket[socket_number]); //closes connection socket
	    }
	}
	closesocket(this->server_socket); //closes server socket
}

//receives messages from the client_socket
char * winsock_client::receive_message() {
	recv(this->client_socket, this->rx_buffer, sizeof(this->rx_buffer), 0);
	return this->rx_buffer;
}

//sends messages to the client_socket
void winsock_client::send_message(char * tx_buffer) {
	send(this->client_socket, tx_buffer, strlen(tx_buffer), 0);
}

//connects to a tcp_server, exits in case the server is unavailable
void winsock_client::connect_to_tcp_server(){
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET; //Address family type internet
	SvrAddr.sin_port = htons(this->port); //port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr(this->ip.c_str()); //IP address
	if ((connect(this->client_socket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(this->client_socket);
		WSACleanup();
		Print("Could not connect to the server");
		std::cin.get();
		exit(0);
	}
}

//connects to a tcp_server, keeps trying at 200ms intervals in case
// the server is unavailable
void winsock_client::connect_to_tcp_server_loop(){
	Print("Trying to connect to the server");

	bool connected = false;
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET; //Address family type internet
	SvrAddr.sin_port = htons(this->port); //port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr(this->ip.c_str()); //IP address
	while (!connected) {
		if ((connect(this->client_socket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		else {
			Print("Connection Established");
			connected = true;
		}
	}
}

//client constructor that sets up the port number and ip number
//it also initializes the socket
winsock_client::winsock_client(int port, std::string ip, std::ofstream *theStream){
	this->ofs = theStream;
	this->port = port;
	this->ip = ip;
	this->client_socket = this->initialize_tcp_socket();
}

//client socket destructor that closes the client_socket
winsock_client::~winsock_client(){
	closesocket(this->client_socket); //closes client socket
}
