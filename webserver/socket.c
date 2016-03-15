#include "socket.h"

const char *motd = "Welcome to the server!\nWe are Potatoes & co.\nPraise our Lord Mousline, the creator of our potatoid world.\nEvery month we sacrifice a potato to thwart our world's destruction.\nTo join us contact us on PotatoBook or by phone at 000 000 008.\nWe are based in Potatoland, 50 potato-salad street, Potatoville.\nSigning up is free if you subscribe to our monthly insurance plan.(*)\nMay the purée be with you.\nMay the Potato Lord protect us.\n(*) Fees up to 5000000 potatobucks may apply.\n";

enum http_method {
	HTTP_GET,
	HTTP_UNSUPPORTED,
};

typedef struct
{
	enum http_method method;
	int major_version;
	int minor_version;
	char *url;
} http_request;


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


char *fgets_or_exit(char *buffer, int size, FILE *stream)
{
    if (fgets(buffer, size, stream) == NULL)
    {
        exit(0);
    }
    else
    {
        return buffer;
    }
}

int parse_http_request(char *request_line, http_request *request) {

	char *word;
	if ((word = strtok(request_line, " ")) != NULL)
		request->method = (strcmp(word, "GET")) ? HTTP_UNSUPPORTED : HTTP_GET;

	if ((word = strtok(NULL, " ")) != NULL)
		request->url = word;

	if ((word = strtok(NULL, " ")) != NULL) {
		if (strlen(word) == 10)
			request->major_version = word[5];
		else
			return -1;
	}

	if ((word = strtok(NULL, " ")) != NULL) {
		if (strlen(word) == 10)
			request->minor_version = word[7];
		else
			return -1;
	}

	return 0;
}

void skip_headers(FILE *client) {
	int buffer_size = 1024;
	char *buffer = calloc(buffer_size, 1);
	char *line;
	do {
		line = fgets_or_exit(buffer, buffer_size, client);
		printf("<Client> %s", line);
	}
	while (!(strcmp(line, "\r\n") == 0 || strcmp(line, "\n") == 0));
}

void send_status(FILE *client, int code, const char *reason) {
	fprintf(client, "HTTP/1.1 %d %s\r\n", code, reason);
}

void send_response(FILE *client, int code, const char *reason, const char *body) {
	send_status(client, code, reason);
	fprintf(client, "Content-Length: %zu\r\n", strlen(body));
	fprintf(client, "\r\n");
	fprintf(client, "%s\r\n", body);
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

			char *line;
			while (1) {
				line = fgets_or_exit(buffer, buffer_size, client);
				printf("<Client> %s", line);

				http_request request;
				if (parse_http_request(line, &request)) {
					skip_headers(client);
					send_response(client, 400, "Bad Request", "400 Bad request");
					exit(0);
				}
				skip_headers(client);
				if (strcmp(request.url, "/") != 0) {
					send_response(client, 404, "Not Found", "404 Not found");
					exit(0);
				}

				send_response(client, 200, "OK", motd);
				exit(0);

				buffer = calloc(buffer_size, 1);
			}
			free(buffer);

		}
		close(client_socket);
	}
}