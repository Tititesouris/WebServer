#include "socket.h"

int main(void)
{
	int server = creer_serveur(8080);
	start(server);
	return 0;
}