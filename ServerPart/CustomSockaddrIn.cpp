#include "CustomSockaddrIn.h"

CUSTOM_SOCKADDR_IN::CUSTOM_SOCKADDR_IN(int socket_in_family, const char* const IP, int port)
{
	inet_pton(socket_in_family, IP, &address.sin_addr.s_addr);
	address.sin_port = htons(port);
	address.sin_family = socket_in_family;

	address_size = sizeof(address);
}