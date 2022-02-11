#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

struct CUSTOM_SOCKADDR_IN
{
	SOCKADDR_IN address;
	int address_size;

	CUSTOM_SOCKADDR_IN(int socket_in_family, const char* const IP, int port);
};