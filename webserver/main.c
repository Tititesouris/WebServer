#include "socket.h"

int main(void)
{
	int server = create_server(8080);
	start(server);
	return 0;
}