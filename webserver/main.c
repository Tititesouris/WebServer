#include "socket.h"

int main(void)
{
	int server = creer_serveur(8080);
	while (server != -1)
	{
		printf("coucou\n");
	}
	return 0;
}