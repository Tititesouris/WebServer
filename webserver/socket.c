#include "socket.h"



int creer_serveur(int port)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		perror("Creating serveur socket:");
		return -1;
	}

	struct sockaddr_in saddr;
	saddr.sin_famity = AF_INET;
	saddr.sin_port = hton(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket,(struct sockaddr *)&saddr, sizeof(inet_in)) == -1)
	{
		perror("Binding server socket address:");
		return -1;
	}
	return socket;
}

