#include "socket.h"

int create_socket(void)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		perror("Creating serveur socket:");
		return -1;
	}
	return server_socket;
}

int bind_socket(int server_socket, int port)
{	
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket,(struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		perror("Binding server socket address:");
		return -1;
	}
	return server_socket;
}

int creer_serveur(int port)
{
	int server_socket;
	if ((server_socket = create_socket()) == -1)
		return -1;
	if (bind_socket(server_socket, port) == -1)
		return -1;
	
	return server_socket;
}
