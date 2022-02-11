#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "CustomSockaddrIn.h"

void CheckWinSockVersion(WORD dll_version, WSADATA* wsa_data)
{
	if (WSAStartup(dll_version, wsa_data) != 0)
	{
		std::cout << "ERROR ";
		std::cout << ">> Can not get WinSock2 appropriate version" << std::endl;

		exit(1);
	}
}

void SafeBind(SOCKET& socket, CUSTOM_SOCKADDR_IN* sock_addr)
{
	if (bind(socket, (SOCKADDR*)&sock_addr->address, sock_addr->address_size) != 0)
	{
		const int START_PORT = 1000;
		const int FINISH_PORT = 1500;

		int port_number = START_PORT;
		sock_addr->address.sin_port = htons(port_number);

		while (bind(socket, (SOCKADDR*)&sock_addr->address, sock_addr->address_size) != 0)
		{
			sock_addr->address.sin_port = htons(++port_number);
			if (port_number > FINISH_PORT) { break; }
		}

		if (port_number > FINISH_PORT)
		{
			std::cout << "ERROR";
			std::cout << " >> Failed to bind socket with following configuration:" << std::endl;
			char IP_address[256];
			inet_ntop(sock_addr->address.sin_family, &sock_addr->address.sin_addr, IP_address, sizeof(IP_address));
			std::cout << "\t adddress: " << IP_address << std::endl;
			std::cout << "\t ports: from " << START_PORT << " to " << FINISH_PORT << std::endl;
			exit(1);
		}
	}

	std::cout << "SUCCESS";
	std::cout << " >> Socket was binded with following configuration:" << std::endl;
	char IP_address[256];
	inet_ntop(sock_addr->address.sin_family, &sock_addr->address.sin_addr, IP_address, sizeof(IP_address));
	std::cout << "\t adddress: " << IP_address << std::endl;
	std::cout << "\t port: " << ntohs(sock_addr->address.sin_port) << std::endl;
	std::cout << "\t family: " << sock_addr->address.sin_family << std::endl;
}

/*struct ClientHandlerData
{
public:
	SOCKET* connections;
	int* counter;
	int* index;

	ClientHandlerData();
	ClientHandlerData(SOCKET* connections, int* counter, int* index);
};
ClientHandlerData::ClientHandlerData(SOCKET* connections, int* counter, int* index)
{
	this->connections = connections;
	this->counter = counter;
	this->index = index;
}
ClientHandlerData::ClientHandlerData()
{
	this->connections = nullptr;
	this->counter = nullptr;
	this->index = nullptr;
}

void ClientHandler(LPVOID* data)
{
	char msg[256];
	ClientHandlerData* tmp_data = (ClientHandlerData*)data;
	std::cout << tmp_data->counter << std::endl;
	while (true)
	{
		recv(tmp_data->connections[*tmp_data->index], msg, sizeof(msg), NULL);
		for (int i = 0; i < *tmp_data->counter; i++)
		{
			if (i == *tmp_data->index) { continue; }

			send(tmp_data->connections[i], msg, sizeof(msg), NULL);
		}
	}
}*/

const int MSG_MAX_LENGTH = 256;

const int CLIENT_MAX_CAPACITY = 100;

SOCKET Connections[CLIENT_MAX_CAPACITY];
int Counter = 0;

void ClientHandler(int index)
{
	char msg[MSG_MAX_LENGTH];
	while (true)
	{
		recv(Connections[index], msg, sizeof(msg), NULL);
		for (int i = 0; i < Counter; i++)
		{
			if (i == index) { continue; }

			send(Connections[i], msg, sizeof(msg), NULL);
		}
	}
}

int main(int argc, char* argv[])
{
	//Library download
	WSAData wsa_data;
	CheckWinSockVersion(MAKEWORD(2, 1), &wsa_data);

	//Create struct with socket network configuration
	CUSTOM_SOCKADDR_IN address(AF_INET, "127.0.0.1\0", 1111);

	//Creates socket
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	//Bind (apply) created socket and configuration
	SafeBind(sListen, &address);

	//Listen to port
	//First param: socket we want to listen
	//Second param: max number of awaiting request
	listen(sListen, SOMAXCONN);

	//Create array for all 100 possible users
	/*SOCKET Connections[100];
	int Counter = 0;
	int index = 0;
	ClientHandlerData* data = new ClientHandlerData(Connections, &Counter, &index);*/

	//Creates socket for handling new connection
	SOCKET newConnection;
	for (int index = 0; index < CLIENT_MAX_CAPACITY; index++)
	{
		newConnection = accept(sListen, (SOCKADDR *)&address.address, &address.address_size);
		//Check if client connected succesfully
		if (newConnection == 0)
		{
			std::cout << "ERROR" << std::endl;
			std::cout << "Can not create connection to client" << std::endl;
		}
		else
		{
			std::cout << "Client Connected Succesfully" << std::endl;
			char msg[MSG_MAX_LENGTH] = "It's the message from server to client";
			send(newConnection, msg, sizeof(msg), NULL);

			Connections[index] = newConnection;
			Counter++;
			/*data->connections[index] = newConnection;
			*(data->counter) = *data->counter + 1;*/
			
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(index), NULL, NULL);
		}
	}

	


	system("pause");
	return 0;
}