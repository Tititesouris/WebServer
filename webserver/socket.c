#include "socket.h"

int create_socket(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Creating serveur socket:");
		return -1;
	}
	return sockfd;
}

int bind_socket(int sockfd, int port)
{	
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd,(struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		perror("Binding server socket address:");
		return -1;
	}
	return sockfd;
}

int listen_socket(int sockfd, int backlog_size)
{
	if (listen(sockfd, backlog_size) == -1)
	{
		perror("Listening to socket:");
		return -1;
	}
	return sockfd;
}

int creer_serveur(int port)
{
	int sockfd;
	if ((sockfd = create_socket()) == -1)
		return -1;
	if (bind_socket(sockfd, port) == -1)
		return -1;
	if (listen_socket(sockfd, 10) == -1)
		return -1;
	return sockfd;
}

int start(int sockfd)
{
	int client;
	client = accept(sockfd, NULL, NULL);
	if (client == -1)
	{
		perror("Accepting client connexion:");
		return -1;
	}
	const char *motd = "Welcome to the server!\n";
	write(client , motd , strlen(motd));
	return client;
}