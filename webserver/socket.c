#include "socket.h"

const char *motd = "Welcome to the server!\nWe are Potatoes & co.\nPraise our Lord Mousline, the creator of our potatoid world.\nEvery month we sacrifice a potato to thwart our world's destruction.\nTo join us contact us on PotatoBook or by phone at 000 000 008.\nWe are based in Potatoland, 50 potato-salad street, Potatoville.\nSigning up is free if you subscribe to our monthly insurance plan.(*)\nMay the purée be with you.\nMay the Potato Lord protect us.\n(*) Fees up to 5000000 potatobucks may apply.\n";

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
	int nb_words = 0;
	char *method;
	char *uri;
	char *version;

	char *word;
	word = strtok(line, " ");
	while (word != NULL)
	{
	 	if (nb_words == 0)
	 		method = word;
	 	else if (nb_words == 1)
	 		uri = word;
	 	else
	 		version = word;
		nb_words++;
	 	word = strtok(NULL, " ");
	}
	printf("Method: %s\n", method);
	printf("URI: %s\n", uri);
	printf("Version: %s\n", version);

	if (strcmp(method, "GET") == 0 && nb_words == 3 && (strcmp(version, "HTTP/1.0\r\n") == 0 || strcmp(version, "HTTP/1.1\r\n") == 0)) {
		if (strcmp(uri, "/") != 0)
			return 404;
		return 200;
	}
	return 400;
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
			int header_code = 0;

			char *line;
			while ((line = fgets(buffer, buffer_size, client)) != NULL) {
				printf("<Client> %s", line);

				if (!first_line_check) {
					header_code = check_http_header(line);
					first_line_check = 1;
				}
				else if (strcmp(line, "\r\n") == 0 || strcmp(line, "\n") == 0 ) {
					if (header_code == 200) {
						fprintf(client, "HTTP/1.1 200 OK\r\n");
						fprintf(client, "Content-Length: %zu\r\n", strlen(motd));
						fprintf(client, "\r\n");
						fprintf(client, motd);
					}
					else if (header_code == 400) {
						fprintf(client, "HTTP/1.1 400 Bad Request\r\n");
						fprintf(client, "Connection: close\r\n");
						fprintf(client, "Content-Length: 17\r\n");
						fprintf(client, "\r\n");
						fprintf(client, "400 Bad request\r\n");
					}
					else if (header_code == 404) {
						fprintf(client, "HTTP/1.1 404 Not Found\r\n");
						fprintf(client, "Connection: close\r\n");
						fprintf(client, "Content-Length: 15\r\n");
						fprintf(client, "\r\n");
						fprintf(client, "404 Not found\r\n");
					}
				}

				buffer = calloc(buffer_size, 1);
			}
			free(buffer);

		}
		close(client_socket);
	}
}