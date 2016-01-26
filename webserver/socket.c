#include "socket.h"



int creer_serveur(int port)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		perror("Creating serveur socket:");
		return -1;
	}
	return port;
}

