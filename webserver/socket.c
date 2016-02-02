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
		int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
		perror("Setting socket options:");
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
	const char *motd = "Welcome to the server!\nWe are Potatoes & co.\nPraise our Lord Mousline, the creator of our potatoid world.\nEvery month we sacrifice a potato to thwart our world's destruction.\nTo join us contact us on PotatoBook or by phone at 000 000 008.\nWe are based in Potatoland, 50 potato-salad street, Potatoville.\nSigning up is free if you subscribe to our monthly insurance plan.(*)\nMay the purée be with you.\nMay the Potato Lord protect us.\n(*) Fees up to 5000000 potatobucks may apply.\n";
	write(client , motd , strlen(motd));

	int buffer_size = 1024;
	int ok = 1;
	while (ok)
	{
		unsigned char *buffer = calloc(buffer_size, 1);
		if (read(client, buffer, buffer_size) > 0)
			write(client, buffer, buffer_size);
		else
			ok = 0;
		free(buffer);
	}
	printf("Connexion stopped.\n");
	return client;
}