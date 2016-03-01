#include "socket.h"

void handle_signal(int sig)
{
	printf("Client disconnected. (%d)\n", sig);
	waitpid(-1, NULL, 0);
}

void initialize_signals(void)
{
	struct sigaction sa;
	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("Initializing SIGCHLD signal:");
	}
}

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

int create_server(int port)
{
	initialize_signals();
	int sockfd;
	if ((sockfd = create_socket()) == -1)
		return -1;
	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
	{
		perror("Setting socket options:");
		return -1;
	}
	if (bind_socket(sockfd, port) == -1)
		return -1;
	if (listen_socket(sockfd, 10) == -1)
		return -1;
	return sockfd;
}

int check_http_header(char *line) {

	// Counting spaces
	int spaces = 0;
	unsigned int i;
	for (i = 0; i < strlen(line); ++i)
	{
		if (line[i] == ' ') {
			spaces++;
		}
	}

	// First word is the method
	char method[4];
	strncpy(method, line, 3);
	method[3] = '\0';

	// Last word is the version
	char version[9];
	strncpy(version, &line[strlen(line) - 9], 8);
	version[8] = '\0';

	return strcmp(method, "GET") == 0 && spaces == 2 && (strcmp(version, "HTTP/1.0") == 0 || strcmp(version, "HTTP/1.1") == 0);
}

int start(int sockfd)
{
	while (1)
	{
		int client_socket;
		client_socket = accept(sockfd, NULL, NULL);
		if (client_socket == -1)
		{
			perror("Accepting client connexion:");
			return -1;
		}
		FILE *client = fdopen(client_socket, "w+");
		printf("Client connected.\n");

		if (!fork())
		{
			

			int buffer_size = 1024;
			char *buffer = calloc(buffer_size, 1);

			int first_line_check = 0;

			char *line;
			while ((line = fgets(buffer, buffer_size, client)) != NULL) {
				printf("<Client> %s", line);

				if (!first_line_check) {
					if (!check_http_header(line)) {
						printf("WOOOOOOOOOOOOOO\n");
						fprintf(client, "HTTP/1.1 400 Bad Request\r\n");
						fprintf(client, "Connection: close\r\n");
						fprintf(client, "Content-Length: 17\r\n");
						fprintf(client, "\r\n");
						fprintf(client, "400 Bad request\r\n");
					}
					first_line_check = 1;
				}

				buffer = calloc(buffer_size, 1);
			}
			free(buffer);
			
				//if (spaces == 2					&& strcmp("GET", words[0]) == 0					&& (strcmp("HTTP/1.0", words[2]) == 0						|| strcmp("HTTP/1.1", words[2]) == 0						)) {
			
			//while (!(buffer[0] == '\n' || (buffer[0] == '\r' && buffer[1] == '\n')))

/*
					printf("YES\n");
					const char *motd = "Welcome to the server!\nWe are Potatoes & co.\nPraise our Lord Mousline, the creator of our potatoid world.\nEvery month we sacrifice a potato to thwart our world's destruction.\nTo join us contact us on PotatoBook or by phone at 000 000 008.\nWe are based in Potatoland, 50 potato-salad street, Potatoville.\nSigning up is free if you subscribe to our monthly insurance plan.(*)\nMay the purée be with you.\nMay the Potato Lord protect us.\n(*) Fees up to 5000000 potatobucks may apply.\n";
					fprintf(client, motd);*/

		}
		close(client_socket);
	}
}